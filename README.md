Author: [@_niklasb](https://github.com/_niklasb)

[Overview article.](https://phoenhex.re/2018-08-26/csgo-fuzzing-bsp)

## Prerequisites

```bash
$ sudo apt install gdb valgrind build-essential python3-minimal python-minimal
$ cd ~
$ git clone https://github.com/niklasb/gdbinit
$ cd gdbinit
$ ./setup.sh
```

Then, build AFL with qemu mode support and `afl_patches.diff` applied. Set
`AFL_PATH` correctly in your `.bashrc`.

## Setup

1. `git clone https://github.com/niklasb/bspfuzz/ && cd bspfuzz`
2. Copy over `bin/` and `csgo/` directories from the CS:GO server installation
   into the `bspfuzz` directory
3. Adapt offsets in `main.cpp` and `patch.py` for your version
4. `./setup.sh`

## Running

```bash
$ cd /path/to/bspfuzz
$ ./run_afl.sh 1
$ ./run_afl.sh 2
$ ./run_afl.sh 3
...
```

## Triaging

```bash
$ sudo sysctl -w kernel.randomize_va_space=0
$ cd /path/to/bspfuzz/triage
$ ./triage.sh
$ ./valgrind.sh
```
