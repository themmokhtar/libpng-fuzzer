#!/bin/sh
# Usage: ./build_libpng_afl++.sh 
# Tested using llvm/clang v17.0.6

. ./afl-config.sh

cd libpng-1.6.43

make clean
./configure --prefix=$PWD/afl++-build
make install

cd ..