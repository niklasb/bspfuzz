#!/bin/bash
set -e
cd "$(dirname "$0")"
ROOT="$(pwd)"
FUZZDIR="$ROOT/.."
FUZZBIN=./bspfuzz

for f in bugid/*; do
  bugid="$(basename "$f")"
  for hash in $(ls "$f" | head -n2); do
    hash="${hash/.gdb.txt/}"
    in="crashes/$hash.bsp"
    out="valgrind/${bugid}_${hash}.txt"
    if [ ! -e "$out" ]; then
      echo
      echo
      echo
      echo "$in => $out"
      echo
      echo
      echo
      pushd "$FUZZDIR" &>/dev/null
      tmpfile="/tmp/valgrind_$$.txt"
      valgrind --undef-value-errors=no "$FUZZBIN" "$ROOT/$in" |& tee "$tmpfile"
      popd &>/dev/null
      mv "$tmpfile" "$ROOT/$out"
    fi
  done
done
