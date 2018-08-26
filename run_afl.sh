#!/bin/bash
if [[ $# < 1 ]] ; then
  echo >&2 "Usage: $0 <workerid>"
  exit 1
fi

NUM=$1
export AFL_ENTRY_POINT=$(nm bspfuzz  |& grep forkserver | cut -d' ' -f1)
export AFL_INST_LIBS=1
echo "Entry point: $AFL_ENTRY_POINT"
if [[ $NUM == 1 ]]; then
  echo Running master
  screen -dmS afl$NUM $AFL_PATH/afl-fuzz -m 2048 -Q -i fuzz/in -o fuzz/out -M fuzzer$NUM -- ./bspfuzz @@
elif [[ $NUM < 9 ]]; then
  echo Running slave
  screen -dmS afl$NUM $AFL_PATH/afl-fuzz -m 2048 -Q -i fuzz/in -o fuzz/out -S fuzzer$NUM -- ./bspfuzz @@
else
  echo 'Running slave (no affinity)'
  export AFL_NO_AFFINITY=1
  screen -dmS afl$NUM $AFL_PATH/afl-fuzz -m 2048 -Q -i fuzz/in -o fuzz/out -S fuzzer$NUM -- ./bspfuzz @@
fi
