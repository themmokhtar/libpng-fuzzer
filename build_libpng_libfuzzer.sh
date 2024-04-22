#!/bin/sh
# Usage: . ./build_libpng_libfuzzer.sh 
# Tested using llvm/clang v17.0.6

cd libpng-1.6.43

make clean
./configure --prefix=$PWD/libfuzzer-build CC=clang CFLAGS="--coverage -g -O1 -fsanitize=fuzzer-no-link,address" LDFLAGS="--coverage -fsanitize=fuzzer-no-link,address"
make install

cd ..