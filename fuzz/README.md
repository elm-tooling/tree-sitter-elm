# Tree-Sitter Elm Scanner Fuzzer

This directory contains a simple fuzzer that hooks directly into the `scanner.c` `scan` function to exercise it with werid 
input. This bypasses the tree-sitter's `tree-sitter scan` flow that probably sanitizes away a lot of weirdness for our scanner. 

Very useful for testing after changes to `scanner.c`. If you only changed the grammar.js file, not need to run this as it
doesn't change `scanner.c` automatically.

## Directory Structure

- `fuzz_scanner.c` - Main fuzzing driver that exercises the scanner
- `Makefile` - Build system for various fuzzer configurations
- `seeds/` - Seed corpus of interesting inputs (checked into git)
- `build/` - Build artifacts (gitignored)

## Prerequisites

### macOS (with Homebrew)

```bash
# Install LLVM with libFuzzer support
brew install llvm

# Install AFL++ (optional, for AFL fuzzing)
brew install afl++

# Install Honggfuzz (optional, for Honggfuzz fuzzing)
brew install honggfuzz
```

### Linux

Install clang with libFuzzer support and AFL++ as appropriate for your distribution.

## Building

### Default Build (AddressSanitizer + UndefinedBehaviorSanitizer)

```bash
make
```

This creates `build/fuzz_scanner` - a standalone fuzzer with sanitizers.

### libFuzzer Build (Recommended)

```bash
make build/fuzz_scanner_libfuzzer
```

This creates `build/fuzz_scanner_libfuzzer` - optimized for continuous fuzzing.

### AFL++ Build

```bash
make build/fuzz_scanner_afl
```

This creates `build/fuzz_scanner_afl` - for AFL++ fuzzing.

### Honggfuzz Build

```bash
make build/fuzz_scanner_honggfuzz
```

This creates `build/fuzz_scanner_honggfuzz` - for Honggfuzz fuzzing.

### Plain Build (No Sanitizers)

```bash
make build/fuzz_scanner_plain
```

This creates `build/fuzz_scanner_plain` - for debugging or Valgrind analysis.

## Running

### Test with a Single File

```bash
make test
```

Runs the fuzzer on `../examples/basic.elm` to verify basic functionality.

### Continuous Fuzzing with libFuzzer

```bash
make fuzz
```

This will:
- Use `seeds/` as read-only seed inputs
- Write new interesting inputs to `build/corpus/`
- Run continuously until interrupted (Ctrl+C)

### AFL++ Fuzzing

```bash
make afl
```

This will:
- Use `seeds/` as initial input corpus
- Write findings to `build/findings/`
- Run AFL++ fuzzing campaign

### Manual Testing

You can also run the fuzzer manually:

```bash
# Test a specific file
./build/fuzz_scanner ../examples/basic.elm

# Test from stdin
echo "foo = bar" | ./build/fuzz_scanner

# libFuzzer with custom options
./build/fuzz_scanner_libfuzzer build/corpus seeds -max_len=4096 -jobs=4
```

## Understanding the Output

### libFuzzer Output

- **NEW** - New code coverage discovered
- **REDUCE** - Input minimized while maintaining coverage
- **pulse** - Progress indicator
- **cov:** - Code coverage statistics
- **corp:** - Corpus size statistics

### Corpus Management

- `seeds/` - Curated seed inputs checked into git
- `build/corpus/` - Live corpus of discovered inputs (gitignored)
- `build/findings/` - AFL++ crash/hang findings (gitignored)

## Adding New Seeds

When you find interesting inputs (crashes, new coverage, etc.):

1. Add them to `seeds/` directory
2. Commit to git for regression testing

## Troubleshooting

### Build Issues

- Ensure LLVM is installed: `brew install llvm`
- Check SDK path: `xcrun --show-sdk-path`
- For AFL++: `brew install afl++`

### Runtime Issues

- **"corpus/" not found**: Run `make fuzz` to create the corpus directory
- **Sanitizer errors**: These are bugs! Report them
- **No new coverage**: Try different input patterns or increase `-max_len`

### Performance

- libFuzzer is generally fastest for continuous fuzzing
- AFL++ excels at finding complex bugs
- Use multiple cores with libFuzzer: `-jobs=4`

## Contributing

When adding new fuzzing capabilities:

1. Update this README
2. Test all build targets
3. Ensure new seeds are added to `seeds/`
4. Update Makefile if needed
