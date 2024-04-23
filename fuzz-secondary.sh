#!/bin/bash
./docker-exec.sh "make fuzz-secondary FUZZ_NAME=$1 PWD=/src/harness AFL_AUTORESUME=1"