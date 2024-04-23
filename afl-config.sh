#!/bin/sh
export CC=afl-cc
export AFL_CC_COMPILER=LTO
export AFL_USE_ASAN=1
export AFL_USE_UBSAN=1
export AFL_LLVM_LAF_SPLIT_SWITCHES=1
export AFL_LLVM_LAF_SPLIT_COMPARES=1
export AFL_LLVM_LAF_TRANSFORM_COMPARES=1

# export AFL_USE_LSAN=1
# export AFL_USE_CFISAN=1
# export AFL_LLVM_CMPLOG=1
# export AFL_LLVM_INSTRUMENT=LTO
# export AFL_LLVM_CTX=1
# export AFL_USE_MSAN=1
# AFL_COMPCOV_LEVEL https://github.com/AFLplusplus/AFLplusplus/blob/stable/qemu_mode/libcompcov/README.md