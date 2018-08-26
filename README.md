## Prerequisites

```
$ sudo apt install gdb valgrind build-essential python3-minimal
$ cd ~
$ git clone https://github.com/niklasb/gdbinit
$ cd gdbinit
$ ./setup.sh
```

Then, build AFL with qemu mode support and `afl_patches.diff` applied. Set
`AFL_PATH` correctly in your `.bashrc`.
