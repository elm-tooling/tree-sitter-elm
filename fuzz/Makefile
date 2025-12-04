# Fuzzing driver Makefile for tree-sitter-elm scanner

CC ?= cc
CFLAGS ?= -g -O0 -Wall -Wextra
SRC_DIR = ../src
BUILD_DIR = build

# Homebrew LLVM (has libFuzzer support)
BREW_LLVM = /opt/homebrew/opt/llvm
BREW_CLANG = $(BREW_LLVM)/bin/clang
MACOS_SDK = $(shell xcrun --show-sdk-path)

# Default target: standalone with sanitizers
all: $(BUILD_DIR)/fuzz_scanner

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Standalone driver with AddressSanitizer and UndefinedBehaviorSanitizer
$(BUILD_DIR)/fuzz_scanner: fuzz_scanner.c $(SRC_DIR)/scanner.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -fsanitize=address,undefined -I$(SRC_DIR) -o $@ fuzz_scanner.c $(SRC_DIR)/scanner.c
	dsymutil $@ -o $@.dSYM

# Plain build without sanitizers (for Frama-C, Valgrind, etc.)
$(BUILD_DIR)/fuzz_scanner_plain: fuzz_scanner.c $(SRC_DIR)/scanner.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -I$(SRC_DIR) -o $@ fuzz_scanner.c $(SRC_DIR)/scanner.c

# libFuzzer build (requires Homebrew LLVM on macOS)
$(BUILD_DIR)/fuzz_scanner_libfuzzer: fuzz_scanner.c $(SRC_DIR)/scanner.c | $(BUILD_DIR)
	$(BREW_CLANG) $(CFLAGS) -isysroot $(MACOS_SDK) -fsanitize=fuzzer,address,undefined -DLIBFUZZER -I$(SRC_DIR) -o $@ fuzz_scanner.c $(SRC_DIR)/scanner.c
	dsymutil $@ -o $@.dSYM

# AFL++ build (set AFL_CC_COMPILER=LLVM and ensure SDK path is correct)
$(BUILD_DIR)/fuzz_scanner_afl: fuzz_scanner.c $(SRC_DIR)/scanner.c | $(BUILD_DIR)
	SDKROOT=$(MACOS_SDK) afl-clang-fast $(CFLAGS) -isysroot $(MACOS_SDK) -I$(SRC_DIR) -o $@ fuzz_scanner.c $(SRC_DIR)/scanner.c
	dsymutil $@ -o $@.dSYM

# Honggfuzz build
$(BUILD_DIR)/fuzz_scanner_honggfuzz: fuzz_scanner.c $(SRC_DIR)/scanner.c | $(BUILD_DIR)
	hfuzz-clang $(CFLAGS) -I$(SRC_DIR) -o $@ fuzz_scanner.c $(SRC_DIR)/scanner.c
	dsymutil $@ -o $@.dSYM

# Seed directory (checked into git) and live corpus directory (gitignored)
SEEDS_DIR = seeds
CORPUS_DIR = $(BUILD_DIR)/corpus

# Run with a single file
test: $(BUILD_DIR)/fuzz_scanner
	$(BUILD_DIR)/fuzz_scanner ../examples/basic.elm

# Run libFuzzer (corpus is live, seeds is read-only)
fuzz: $(BUILD_DIR)/fuzz_scanner_libfuzzer
	mkdir -p $(CORPUS_DIR)
	$(BUILD_DIR)/fuzz_scanner_libfuzzer $(CORPUS_DIR) $(SEEDS_DIR) -max_len=4096

# Run AFL++
afl: $(BUILD_DIR)/fuzz_scanner_afl
	mkdir -p $(CORPUS_DIR) $(BUILD_DIR)/findings
	afl-fuzz -i $(SEEDS_DIR) -o $(BUILD_DIR)/findings $(BUILD_DIR)/fuzz_scanner_afl @@

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all test fuzz afl clean
