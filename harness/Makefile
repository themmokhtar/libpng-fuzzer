# Makefile to compile main.c with libpng (Tested using llvm/clang v17.0.6)
# CC = clang
# CFLAGS = -Wall -Werror --coverage -g -O1 -fsanitize=fuzzer,address,undefined -I../libpng-1.6.43/libfuzzer-build/include
# LDFLAGS = -lz --coverage -g -O1 -fsanitize=fuzzer,address,undefined -L../libpng-1.6.43/libfuzzer-build/lib -lpng16

CFLAGS = -Wall -Werror -I../libpng-1.6.43/libfuzzer-build/include
LDFLAGS = -L../libpng-1.6.43/libfuzzer-build/lib -lpng16

# export CC=afl-cc
# export AFL_CC_COMPILER=LTO
# export AFL_USE_ASAN=1
# export AFL_USE_UBSAN=1
export CC=afl-cc
export AFL_CC_COMPILER=LTO
export AFL_USE_ASAN=1
export AFL_USE_UBSAN=1
export AFL_LLVM_LAF_SPLIT_SWITCHES=1
export AFL_LLVM_LAF_SPLIT_COMPARES=1
export AFL_LLVM_LAF_TRANSFORM_COMPARES=1

FUZZ_FORK_COUNT = 6
COVERAGE_REPORT_DELAY = 60

BIN_DIR = $(PWD)/bin
CORPUS_DIR = $(PWD)/corpus
ARTIFACT_DIR = $(PWD)/artifacts
REPORT_DIR = $(PWD)/gcovr

FUZZ_NAME?=1

# MINIMIZED_CORPUS_DIR = $(PWD)/minimized-corpus
# OLD_CORPUS_DIR = $(PWD)/old-corpus

default: build

.PHONY: default build fuzz generate-coverage-report open-coverage-report clean

build: $(BIN_DIR)/main

fuzz: $(BIN_DIR)/main
# LD_LIBRARY_PATH=../libpng-1.6.43/libfuzzer-build/lib $(BIN_DIR)/main -artifact_prefix=$(ARTIFACT_DIR)/ -fork=$(FUZZ_FORK_COUNT) -max_len=1048576 $(CORPUS_DIR)
	mkdir -p $(CORPUS_DIR)
	mkdir -p $(ARTIFACT_DIR)

	afl-fuzz -i $(CORPUS_DIR) -o $(ARTIFACT_DIR) -M main-fuzzer -- $(BIN_DIR)/main

fuzz-secondary: $(BIN_DIR)/main
	afl-fuzz -i $(CORPUS_DIR) -o $(ARTIFACT_DIR) -S fuzzer-$(FUZZ_NAME) -- $(BIN_DIR)/main

clean:
	rm -rf $(BIN_DIR)/*
# rm -rf $(ARTIFACT_DIR)/*
# rm -rf $(REPORT_DIR)/*
# find .. -name "*.gcda" -delete

$(BIN_DIR)/main: $(BIN_DIR)/main.o
	$(CC) $(LDFLAGS) -o $@ $^
# rm -rf ./bin/main-cov.gcda

$(BIN_DIR)/main.o: main.c
	$(CC) $(CFLAGS) -o $@ -c $<

## These are a bunch of snippets that I might still use to update this Makefile:
## Yeah, I'm a hoarder...

# GCOVR = gcovr --gcov-executable "llvm-cov gcov"
# VALGRIND = valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind-out.txt
# COV_CFLAGS = -DCOVERAGE_TESTER -Wall -Werror --coverage -g -O0 -fsanitize=address -I../libpng-1.6.43/coverage-build/include
# COV_LDFLAGS = -lz --coverage -g -O0 -fsanitize=address -Wl,--whole-archive -L../libpng-1.6.43/coverage-build/lib -lpng16 -Wl,--no-whole-archive
# GCOVR_JSON_PATH = $(GCOVR_PATH)/json

# $(BIN_DIR)/main-cov: $(BIN_DIR)/main-cov.o
# 	$(CC) $(COV_LDFLAGS) -o $@ $^ # -Wl,--verbose

# $(BIN_DIR)/main-cov.o: main.c
# 	$(CC) $(COV_CFLAGS) -o $@ -c $<

# define gen-cov-report
# 	@#$(GCOVR) --txt --html-details --html $(GCOVR_PATH)/index.html --root ../libpng-1.6.43 ../libpng-1.6.43/.libs
# 	@#$(GCOVR) --json $(GCOVR_PATH)/libpng-cov.json --root ../libpng-1.6.43 ../libpng-1.6.43/.libs
# 	@#$(GCOVR) --json $(GCOVR_PATH)/harness-cov.json --root . .
# 	@#gcovr --json-add-tracefile $(GCOVR_PATH)/libpng-cov.json --json-add-tracefile $(GCOVR_PATH)/harness-cov.json --html-details --html $(GCOVR_PATH)/index.html
# 	@#$(GCOVR) --html-details --html $(GCOVR_PATH)/index.html -f . -f ../libpng-1.6.43/.libs --root .. ../libpng-1.6.43/.libs .
	
# 	$(GCOVR) --html-details --html $(GCOVR_PATH)/index.html -f . -f ../libpng-1.6.43/.libs --root .. ../libpng-1.6.43/.libs .
# 	@echo "Coverage report generated at $(PWD)/gcovr.html"
# endef

# build-cov: $(BIN_DIR)/main-cov

# define run-cov-target
# 	LD_LIBRARY_PATH=../libpng-1.6.43/coverage-build/lib $(BIN_DIR)/main-cov $(1)

# endef

# run-cov: $(BIN_DIR)/main-cov
# ifndef COV_INPUT
# 	$(error COV_INPUT is not set: Pick a PNG file to test coverage on)
# endif

# 	$(call run-cov-target,$(COV_INPUT))

# cov-random:
# 	$(call run-cov-target, $(shell find ./corpus -type f | shuf -n 1))
# 	$(call gen-cov-report)

# cov-corpus:
# 	$(foreach file, $(wildcard ./corpus/*), $(call run-cov-target,$(file)))
# 	$(call gen-cov-report)

# .PHONY: clean build-fuzz build-cov run-cov run-fuzz cov cov-random cov-corpus