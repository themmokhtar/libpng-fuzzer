ROOT_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

# HONGGFUZZ
HFUZZ_ROOT := $(ROOT_DIR)/honggfuzz

# LIBPNG
LIBPNG_VERSION := 1.6.43

LIBPNG_SRC_ARCHIVE := $(ROOT_DIR)/libpng.tar.xz
LIBPNG_SRC_URL := https://netix.dl.sourceforge.net/project/libpng/libpng16/$(LIBPNG_VERSION)/libpng-$(LIBPNG_VERSION).tar.xz

LIBPNG_FUZZ_ROOT := $(ROOT_DIR)/libpng-fuzz
LIBPNG_FUZZ_BUILD := $(LIBPNG_FUZZ_ROOT)/build
LIBPNG_FUZZ_LIB := $(LIBPNG_FUZZ_BUILD)/lib

# HARNESS
HARNESS_ROOT := $(ROOT_DIR)/harness
HARNESS_BUILD := $(HARNESS_ROOT)/build
HARNESS_SRC := $(wildcard $(HARNESS_ROOT)/*.c)
HARNESS_HDR := $(wildcard $(HARNESS_ROOT)/*.h)
HARNESS_BIN := $(HARNESS_BUILD)/harness

# FUZZING
CORPUS_DIR := $(ROOT_DIR)/corpus
CAMPAIGN_DIR := $(ROOT_DIR)/campaign

# EXPORTS
export CC := $(HFUZZ_ROOT)/hfuzz_cc/hfuzz-clang
export CXX := $(HFUZZ_ROOT)/hfuzz_cc/hfuzz-clang++
export LD_LIBRARY_PATH=$(LIBPNG_FUZZ_LIB)

default: all

.PHONY: all build clean libpng-fuzz-build libpng-fuzz-clean libpng-fuzz-rebuild harness-build harness-clean harness-rebuild harness-run

all: build

build: libpng-fuzz-build harness-build

clean: libpng-fuzz-clean harness-clean

#################
## LIBPNG FUZZ ##
#################

libpng-fuzz-build: $(LIBPNG_FUZZ_ROOT)
	@echo "=> Configuring libpng for fuzzing"
	cd $(LIBPNG_FUZZ_ROOT) && ./configure --prefix=$(LIBPNG_FUZZ_BUILD)

	@echo "=> Building libpng for fuzzing"
	cd $(LIBPNG_FUZZ_ROOT) && $(MAKE) install

libpng-fuzz-clean:
	@echo "=> Cleaning libpng fuzz build"
	cd $(LIBPNG_FUZZ_ROOT) && $(MAKE) clean

libpng-fuzz-rebuild: libpng-fuzz-clean libpng-fuzz-build

$(LIBPNG_SRC_ARCHIVE):
	@echo "=> Downloading libpng source code"
	wget -O $(LIBPNG_SRC_ARCHIVE) $(LIBPNG_SRC_URL)

$(LIBPNG_FUZZ_ROOT): $(LIBPNG_SRC_ARCHIVE)
	@echo "=> Extracting libpng source code to $(LIBPNG_FUZZ_ROOT)"
	tar -xf $(LIBPNG_SRC_ARCHIVE)
	mv $(ROOT_DIR)/libpng-$(LIBPNG_VERSION) $(LIBPNG_FUZZ_ROOT)


#############
## HARNESS ##
#############

harness-build: $(HARNESS_BIN)

harness-clean:
	@echo "=> Cleaning harness build"
	rm -rf $(HARNESS_BUILD)

harness-rebuild: harness-clean harness-build

harness-run: harness-build
ifndef HARNESS_PARAMS
	$(error HARNESS_PARAMS is not set: `make harness-run HARNESS_PARAMS="..."`)
endif

	@echo "=> Running harness"
	$(HARNESS_BIN) $(HARNESS_PARAMS)

$(HARNESS_BIN): $(HARNESS_SRC) $(HARNESS_HDR)
	@echo "=> Building harness"
	mkdir -p $(HARNESS_BUILD)
	$(CC) -o $(HARNESS_BIN) $(HARNESS_SRC) -I$(LIBPNG_FUZZ_BUILD)/include -L$(LIBPNG_FUZZ_LIB) -lpng

#############
## FUZZING ##
#############

fuzz: harness-build
	@echo "=> Starting fuzzer"
	mkdir -p $(CORPUS_DIR) $(CAMPAIGN_DIR)
	$(HFUZZ_ROOT)/honggfuzz -i $(CORPUS_DIR) -o $(CAMPAIGN_DIR) -n$(shell nproc) -- $(HARNESS_BIN) ___FILE___ /dev/null

minimize-corpus:
	@echo "=> Minimizing campaign corpus"
	mkdir -p $(CORPUS_DIR) $(CAMPAIGN_DIR)
	$(HFUZZ_ROOT)/honggfuzz -i $(CAMPAIGN_DIR) -M -- $(HARNESS_BIN) ___FILE___ /dev/null