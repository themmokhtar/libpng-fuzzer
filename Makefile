ROOT_DIR := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))

# Harness
HARNESS_ROOT := $(ROOT_DIR)/harness
HARNESS_SRC := $(wildcard $(HARNESS_ROOT)/*.c)
HARNESS_HDR := $(wildcard $(HARNESS_ROOT)/*.h)

# Honggfuzz
HFUZZ_ROOT := $(ROOT_DIR)/honggfuzz

# LibPNG 
LIBPNG_MAJOR_VERSION := 16
LIBPNG_VERSION := 1.6.43

LIBPNG_SRC_ARCHIVE := $(ROOT_DIR)/libpng.tar.xz
LIBPNG_SRC_URL := https://netix.dl.sourceforge.net/project/libpng/libpng$(LIBPNG_MAJOR_VERSION)/$(LIBPNG_VERSION)/libpng-$(LIBPNG_VERSION).tar.xz

# Probing settings
PROBE_LIBPNG_ROOT := $(ROOT_DIR)/probe-libpng
PROBE_LIBPNG_BUILD := $(PROBE_LIBPNG_ROOT)/build
PROBE_LIBPNG_LIB := $(PROBE_LIBPNG_BUILD)/lib

PROBE_HARNESS_BUILD := $(HARNESS_ROOT)/probe-build
PROBE_HARNESS_BIN := $(PROBE_HARNESS_BUILD)/harness

PROBE_REPORT_DIR := $(ROOT_DIR)/probe-report

PROBE_CC := clang
PROBE_CFLAGS := -g -O1 -fsanitize=address,undefined -fsanitize-address-use-after-return=always -fno-omit-frame-pointer --coverage
PROBE_LD_LIBRARY_PATH=$(PROBE_LIBPNG_LIB)

PROBE_COV_LOCATIONS := $(PROBE_LIBPNG_ROOT) $(PROBE_HARNESS_BUILD)

# Fuzzing settings
FUZZ_CORPUS_DIR := $(ROOT_DIR)/corpus
FUZZ_CAMPAIGN_DIR := $(ROOT_DIR)/campaign
FUZZ_REPORT_DIR := $(ROOT_DIR)/report

FUZZ_HARNESS_BUILD := $(HARNESS_ROOT)/fuzz-build
FUZZ_HARNESS_BIN := $(FUZZ_HARNESS_BUILD)/harness

FUZZ_LIBPNG_ROOT := $(ROOT_DIR)/fuzz-libpng
FUZZ_LIBPNG_BUILD := $(FUZZ_LIBPNG_ROOT)/build
FUZZ_LIBPNG_LIB := $(FUZZ_LIBPNG_BUILD)/lib

FUZZ_REPORT_DIR := $(ROOT_DIR)/fuzz-report

FUZZ_CC := $(HFUZZ_ROOT)/hfuzz_cc/hfuzz-clang
FUZZ_CFLAGS := -g -O1 -fsanitize=address,undefined -fsanitize-address-use-after-return=always -fno-omit-frame-pointer --coverage
FUZZ_LD_LIBRARY_PATH=$(FUZZ_LIBPNG_LIB)

FUZZ_COV_LOCATIONS := $(FUZZ_LIBPNG_ROOT) $(FUZZ_HARNESS_BUILD)

default: all

all: build

build: build-probe build-fuzz
clean: clean-probe clean-fuzz
rebuild: clean build
report: report-probe report-fuzz
cleancov: cleancov-probe cleancov-fuzz

.PHONY: default all build clean rebuild

############
## LIBPNG ##
############

$(LIBPNG_SRC_ARCHIVE):
	@echo "=> Downloading libpng source code"
	wget -O $(LIBPNG_SRC_ARCHIVE) $(LIBPNG_SRC_URL)

#############
## PROBING ##
#############

# *-probe
build-probe: build-probe-libpng $(PROBE_HARNESS_BIN)

clean-probe: clean-probe-libpng
	rm -rf $(PROBE_HARNESS_BUILD)

rebuild-probe: clean-probe build-probe

report-probe:
	@echo "=> Generating probing coverage report"
	mkdir -p $(PROBE_REPORT_DIR)
	gcovr --gcov-executable "llvm-cov gcov" --sort uncovered-percent --html-details --html $(PROBE_REPORT_DIR)/index.html --root $(ROOT_DIR) -f $(HARNESS_ROOT) -f $(PROBE_LIBPNG_ROOT) -x $(PROBE_LIBPNG_ROOT)/a-conftest.gcno $(PROBE_LIBPNG_ROOT) $(PROBE_HARNESS_BUILD)
	@echo "-> Coverage report generated at $(PROBE_REPORT_DIR)/index.html"

cleancov-probe:
	@echo "=> Cleaning probing coverage data"
	find $(PROBE_COV_LOCATIONS) -name "*.gcda" -delete

run-probe: build-probe-harness
ifndef HARNESS_PARAMS
	$(error HARNESS_PARAMS is not set: `make $@ HARNESS_PARAMS="..."`)
endif

	@echo "=> Running probing harness"
	export LD_LIBRARY_PATH=$(PROBE_LD_LIBRARY_PATH) && \
	$(PROBE_HARNESS_BIN) $(HARNESS_PARAMS)

report-probe-run: cleancov-probe run-probe report-probe

.PHONY: build-probe clean-probe rebuild-probe run-probe

# *-probe-libpng
build-probe-libpng: $(PROBE_LIBPNG_ROOT)
	@echo "=> Configuring libpng for probing"
	cd $(PROBE_LIBPNG_ROOT) && ./configure --prefix=$(PROBE_LIBPNG_BUILD) CC=$(PROBE_CC) CFLAGS="$(PROBE_CFLAGS)"

	@echo "=> Building libpng for probing"
	cd $(PROBE_LIBPNG_ROOT) && $(MAKE) install CC=$(PROBE_CC) CFLAGS="$(PROBE_CFLAGS)"

clean-probe-libpng: $(PROBE_LIBPNG_ROOT)
	@echo "=> Cleaning libpng probing build"
	cd $(PROBE_LIBPNG_ROOT) && $(MAKE) clean

rebuild-probe-libpng: clean-probe-libpng build-probe-libpng

.PHONY: build-probe-libpng clean-probe-libpng rebuild-probe-libpng

$(PROBE_LIBPNG_ROOT): $(LIBPNG_SRC_ARCHIVE)
	@echo "=> Extracting libpng source code to $(PROBE_LIBPNG_ROOT)"
	tar -xf $(LIBPNG_SRC_ARCHIVE)
	mv $(ROOT_DIR)/libpng-$(LIBPNG_VERSION) $(PROBE_LIBPNG_ROOT)

# *-probe-harness
build-probe-harness: $(PROBE_HARNESS_BIN)

clean-probe-harness:
	rm -rf $(PROBE_HARNESS_BUILD)

rebuild-probe-harness: clean-probe-harness build-probe-harness

.PHONY: build-probe-harness clean-probe-harness rebuild-probe-harness

$(PROBE_HARNESS_BIN): $(HARNESS_SRC) $(HARNESS_HDR)
	@echo "=> Building harness for probing"
	mkdir -p $(PROBE_HARNESS_BUILD)
	$(PROBE_CC) $(PROBE_CFLAGS) -o $(PROBE_HARNESS_BIN) $(HARNESS_SRC) -I$(PROBE_LIBPNG_BUILD)/include -L$(PROBE_LIBPNG_LIB) -lpng

#############
## FUZZING ##
#############

# *-fuzz
build-fuzz: build-fuzz-libpng $(FUZZ_HARNESS_BIN)

clean-fuzz: clean-fuzz-libpng
	rm -rf $(FUZZ_HARNESS_BUILD)

rebuild-fuzz: clean-fuzz build-fuzz

report-fuzz:
	@echo "=> Generating fuzzing coverage report"
	mkdir -p $(FUZZ_REPORT_DIR)
	gcovr --gcov-executable "llvm-cov gcov" --sort uncovered-percent --html-details --html $(FUZZ_REPORT_DIR)/index.html --root $(ROOT_DIR) -f $(HARNESS_ROOT) -f $(FUZZ_LIBPNG_ROOT) -x $(FUZZ_LIBPNG_ROOT)/a-conftest.gcno $(FUZZ_LIBPNG_ROOT) $(FUZZ_HARNESS_BUILD)
	@echo "-> Coverage report generated at $(FUZZ_REPORT_DIR)/index.html"

cleancov-fuzz:
	@echo "=> Cleaning fuzzing coverage data"
	find $(FUZZ_COV_LOCATIONS) -name "*.gcda" -delete

run-fuzz: build-fuzz-harness $(FUZZ_CAMPAIGN_DIR)
	@echo "=> Starting Honggfuzz"
	export LD_LIBRARY_PATH=$(FUZZ_LD_LIBRARY_PATH) && \
	export ASAN_OPTIONS=detect_stack_use_after_return=1 && \
	$(HFUZZ_ROOT)/honggfuzz -t3 -i $(FUZZ_CAMPAIGN_DIR) -n$(shell nproc) -- $(FUZZ_HARNESS_BIN) ___FILE___ /dev/null

run-fuzz-minimize: build-fuzz-harness $(FUZZ_CAMPAIGN_DIR)
	@echo "=> Starting Honggfuzz minimization"
	export LD_LIBRARY_PATH=$(FUZZ_LD_LIBRARY_PATH) && \
	export ASAN_OPTIONS=detect_stack_use_after_return=1 && \
	$(HFUZZ_ROOT)/honggfuzz -t3 -i $(FUZZ_CAMPAIGN_DIR) -n$(shell nproc) -M -- $(FUZZ_HARNESS_BIN) ___FILE___ /dev/null

$(FUZZ_CAMPAIGN_DIR): 
	@echo "=> Creating campaign directory"
	cp -r $(FUZZ_CORPUS_DIR) $(FUZZ_CAMPAIGN_DIR)

.PHONY: build-fuzz clean-fuzz rebuild-fuzz run-fuzz

# *-fuzz-libpng
build-fuzz-libpng: $(FUZZ_LIBPNG_ROOT)
	@echo "=> Configuring libpng for fuzzing"
	cd $(FUZZ_LIBPNG_ROOT) && ./configure --prefix=$(FUZZ_LIBPNG_BUILD) CC=$(FUZZ_CC) CFLAGS="$(FUZZ_CFLAGS)"

	@echo "=> Building libpng for fuzzing"
	cd $(FUZZ_LIBPNG_ROOT) && $(MAKE) install CC=$(FUZZ_CC) CFLAGS="$(FUZZ_CFLAGS)"

clean-fuzz-libpng: $(FUZZ_LIBPNG_ROOT)
	@echo "=> Cleaning libpng fuzzing build"
	cd $(FUZZ_LIBPNG_ROOT) && $(MAKE) clean

rebuild-fuzz-libpng: clean-fuzz-libpng build-fuzz-libpng

.PHONY: build-fuzz-libpng clean-fuzz-libpng rebuild-fuzz-libpng

$(FUZZ_LIBPNG_ROOT): $(LIBPNG_SRC_ARCHIVE)
	@echo "=> Extracting libpng source code to $(FUZZ_LIBPNG_ROOT)"
	tar -xf $(LIBPNG_SRC_ARCHIVE)
	mv $(ROOT_DIR)/libpng-$(LIBPNG_VERSION) $(FUZZ_LIBPNG_ROOT)

# *-fuzz-harness
build-fuzz-harness: $(FUZZ_HARNESS_BIN)

clean-fuzz-harness:
	rm -rf $(FUZZ_HARNESS_BUILD)

rebuild-fuzz-harness: clean-fuzz-harness build-fuzz-harness

.PHONY: build-fuzz-harness clean-fuzz-harness rebuild-fuzz-harness

$(FUZZ_HARNESS_BIN): $(HARNESS_SRC) $(HARNESS_HDR)
	@echo "=> Building harness for fuzzing"
	mkdir -p $(FUZZ_HARNESS_BUILD)
	$(FUZZ_CC) $(FUZZ_CFLAGS) -o $(FUZZ_HARNESS_BIN) $(HARNESS_SRC) -I$(FUZZ_LIBPNG_BUILD)/include -L$(FUZZ_LIBPNG_LIB) -lpng

