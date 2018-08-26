#!/bin/bash
set -ex
cp bin/dedicated.{,orig.}so
cp bin/engine.{,orig.}so
cp bin/libtier0.{,orig.}so
make
make patch
mkdir -p fuzz/{in,out}
cp mini_bsp/test_mini.bsp fuzz/in
