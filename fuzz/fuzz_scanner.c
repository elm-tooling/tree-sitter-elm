/**
 * Fuzzing driver for tree-sitter-elm scanner
 * 
 * Build:
 *   cc -g -O0 -fsanitize=address,undefined fuzz_scanner.c ../src/scanner.c -o fuzz_scanner
 * 
 * Run with file:
 *   ./fuzz_scanner input.elm
 * 
 * Run with stdin:
 *   echo "foo = bar" | ./fuzz_scanner
 * 
 * For AFL++:
 *   afl-clang-fast -g -O0 fuzz_scanner.c ../src/scanner.c -o fuzz_scanner_afl
 *   afl-fuzz -i corpus -o findings ./fuzz_scanner_afl @@
 * 
 * For libFuzzer:
 *   clang -g -O0 -fsanitize=fuzzer,address,undefined -DLIBFUZZER fuzz_scanner.c ../src/scanner.c -o fuzz_scanner_libfuzzer
 *   ./fuzz_scanner_libfuzzer corpus/
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Include tree-sitter's parser.h for proper types
#include "tree_sitter/parser.h"

// External scanner API (defined in scanner.c)
void *tree_sitter_elm_external_scanner_create(void);
void tree_sitter_elm_external_scanner_destroy(void *payload);
unsigned tree_sitter_elm_external_scanner_serialize(void *payload, char *buffer);
void tree_sitter_elm_external_scanner_deserialize(void *payload, const char *buffer, unsigned length);
bool tree_sitter_elm_external_scanner_scan(void *payload, TSLexer *lexer, const bool *valid_symbols);

// Mock lexer state
typedef struct {
    const uint8_t *data;
    size_t len;
    size_t pos;
    size_t mark_pos;
    int32_t lookahead;
    TSSymbol result_symbol;
} MockLexer;

// Global mock lexer state (accessed via TSLexer callbacks)
static MockLexer g_mock;

// Decode UTF-8 character at position, return codepoint or -1 for invalid
static int32_t decode_utf8(const uint8_t *data, size_t len, size_t pos, size_t *bytes_read) {
    if (pos >= len) {
        *bytes_read = 0;
        return 0; // EOF
    }
    
    uint8_t b0 = data[pos];
    
    // ASCII
    if (b0 < 0x80) {
        *bytes_read = 1;
        return b0;
    }
    
    // Invalid start bytes
    if (b0 < 0xC0 || b0 > 0xF7) {
        *bytes_read = 1;
        return -1; // Invalid UTF-8
    }
    
    // 2-byte sequence
    if (b0 < 0xE0) {
        if (pos + 1 >= len) { *bytes_read = 1; return -1; }
        uint8_t b1 = data[pos + 1];
        if ((b1 & 0xC0) != 0x80) { *bytes_read = 1; return -1; }
        *bytes_read = 2;
        return ((b0 & 0x1F) << 6) | (b1 & 0x3F);
    }
    
    // 3-byte sequence
    if (b0 < 0xF0) {
        if (pos + 2 >= len) { *bytes_read = 1; return -1; }
        uint8_t b1 = data[pos + 1];
        uint8_t b2 = data[pos + 2];
        if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80) { *bytes_read = 1; return -1; }
        *bytes_read = 3;
        return ((b0 & 0x0F) << 12) | ((b1 & 0x3F) << 6) | (b2 & 0x3F);
    }
    
    // 4-byte sequence
    if (pos + 3 >= len) { *bytes_read = 1; return -1; }
    uint8_t b1 = data[pos + 1];
    uint8_t b2 = data[pos + 2];
    uint8_t b3 = data[pos + 3];
    if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) {
        *bytes_read = 1;
        return -1;
    }
    *bytes_read = 4;
    return ((b0 & 0x07) << 18) | ((b1 & 0x3F) << 12) | ((b2 & 0x3F) << 6) | (b3 & 0x3F);
}

static void update_lookahead(void) {
    size_t bytes_read;
    g_mock.lookahead = decode_utf8(g_mock.data, g_mock.len, g_mock.pos, &bytes_read);
    // Handle invalid UTF-8 by returning the raw byte value (what tree-sitter does)
    if (g_mock.lookahead == -1 && g_mock.pos < g_mock.len) {
        g_mock.lookahead = g_mock.data[g_mock.pos];
    }
}

static void mock_advance(TSLexer *lexer, bool skip) {
    (void)skip;
    if (g_mock.pos < g_mock.len) {
        size_t bytes_read;
        decode_utf8(g_mock.data, g_mock.len, g_mock.pos, &bytes_read);
        if (bytes_read == 0) bytes_read = 1;
        g_mock.pos += bytes_read;
    }
    update_lookahead();
    lexer->lookahead = g_mock.lookahead;
}

static void mock_mark_end(TSLexer *lexer) {
    (void)lexer;
    g_mock.mark_pos = g_mock.pos;
}

static uint32_t mock_get_column(TSLexer *lexer) {
    (void)lexer;
    // Find start of current line
    size_t line_start = g_mock.pos;
    while (line_start > 0 && g_mock.data[line_start - 1] != '\n') {
        line_start--;
    }
    
    // Count columns (characters, not bytes) from line start to current position
    // This mimics tree-sitter's get_column which decodes UTF-8
    uint32_t column = 0;
    size_t scan_pos = line_start;
    while (scan_pos < g_mock.pos) {
        size_t bytes_read;
        int32_t cp = decode_utf8(g_mock.data, g_mock.len, scan_pos, &bytes_read);
        (void)cp;
        if (bytes_read == 0) break;
        scan_pos += bytes_read;
        column++;
    }
    return column;
}

static bool mock_is_at_included_range_start(const TSLexer *lexer) {
    (void)lexer;
    return false;
}

static bool mock_eof(const TSLexer *lexer) {
    (void)lexer;
    return g_mock.pos >= g_mock.len;
}

// Initialize mock lexer with input data
static void init_mock_lexer(TSLexer *lexer, const uint8_t *data, size_t len) {
    g_mock.data = data;
    g_mock.len = len;
    g_mock.pos = 0;
    g_mock.mark_pos = 0;
    g_mock.result_symbol = 0;
    
    update_lookahead();
    
    lexer->lookahead = g_mock.lookahead;
    lexer->result_symbol = 0;
    lexer->advance = mock_advance;
    lexer->mark_end = mock_mark_end;
    lexer->get_column = mock_get_column;
    lexer->is_at_included_range_start = mock_is_at_included_range_start;
    lexer->eof = mock_eof;
}

// Fuzz one input
static int fuzz_one(const uint8_t *data, size_t len) {
    TSLexer lexer;
    
    // Try different combinations of valid_symbols
    bool valid_symbols[7];
    
    // Run multiple passes with different valid_symbols configurations
    for (int config = 0; config < 128; config++) {
        // Create fresh scanner for each config to avoid state accumulation
        void *scanner = tree_sitter_elm_external_scanner_create();
        if (!scanner) continue;
        
        // Reset lexer position for each config
        init_mock_lexer(&lexer, data, len);
        
        // Set valid_symbols based on config bits
        for (int i = 0; i < 7; i++) {
            valid_symbols[i] = (config >> i) & 1;
        }
        
        // Call scan multiple times to exercise different states
        int max_iterations = (int)len + 10;
        for (int iter = 0; iter < max_iterations && g_mock.pos < len; iter++) {
            bool result = tree_sitter_elm_external_scanner_scan(scanner, &lexer, valid_symbols);
            (void)result;
            
            // Advance at least one position to avoid infinite loops
            if (g_mock.pos == 0 || (iter > 0 && g_mock.pos < len)) {
                mock_advance(&lexer, true);
            }
        }
        
        tree_sitter_elm_external_scanner_destroy(scanner);
    }
    
    return 0;
}

#ifdef LIBFUZZER
// libFuzzer entry point
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
    fuzz_one(data, size);
    return 0;
}
#else
// Standalone driver
int main(int argc, char *argv[]) {
    uint8_t *data = NULL;
    size_t len = 0;
    
    if (argc > 1) {
        // Read from file
        FILE *f = fopen(argv[1], "rb");
        if (!f) {
            perror("fopen");
            return 1;
        }
        fseek(f, 0, SEEK_END);
        len = (size_t)ftell(f);
        fseek(f, 0, SEEK_SET);
        data = malloc(len + 1);
        if (!data) {
            fclose(f);
            return 1;
        }
        if (fread(data, 1, len, f) != len) {
            fclose(f);
            free(data);
            return 1;
        }
        data[len] = 0;
        fclose(f);
    } else {
        // Read from stdin
        size_t cap = 4096;
        data = malloc(cap);
        if (!data) return 1;
        
        int c;
        while ((c = getchar()) != EOF) {
            if (len >= cap) {
                cap *= 2;
                uint8_t *new_data = realloc(data, cap);
                if (!new_data) {
                    free(data);
                    return 1;
                }
                data = new_data;
            }
            data[len++] = (uint8_t)c;
        }
    }
    
    printf("Fuzzing %zu bytes...\n", len);
    int result = fuzz_one(data, len);
    printf("Done.\n");
    
    free(data);
    return result;
}
#endif
