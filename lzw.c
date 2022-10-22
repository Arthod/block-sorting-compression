/*
Lempel Ziv Welch Algorithm
*/

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef struct Code {
    uint64_t index_start;
    uint64_t index_end;
    uint16_t symbol_value;
} Code;

void lzw_encode(uint16_t *block, uint64_t block_size) {
    Code *dictionary = malloc(65536 * sizeof(Code));
    for (int i = 0; i < 65536; i++) {
        Code *code = malloc(sizeof(Code));
        code->index_start = NULL;
        code->index_end = NULL;
        code->symbol_value = NULL;
        dictionary[i] = *code;
    }
    /*for (int i = 0; i < 256; i++) {
        Code *code = malloc(sizeof(Code));
        code->index_start = NULL;
        code->index_end = NULL;
        dictionary[i] = *code;
    }*/
    
    uint16_t block_new = malloc(block_size * sizeof(uint16_t));
    uint64_t block_size_new = 0;

    for (uint64_t i = 0; i < block_size; i++) {
        uint64_t index_start = i;
        uint64_t index_end = i + 1;

        // Check if character is in dictionary, if not, add it
        Code code = dictionary[block[i]];
        if (code.symbol_value != block[i]) {
            code.index_start = i;
            code.index_end = i;
            code.symbol_value = block[i];
        }

        while (1) {
            // Check if symbols from index_start to index_end are in dictionary
            for (uint64_t j = 0; j < 65536; j++) {
                Code code = dictionary[j];
                if (code.index_start == NULL) {
                    break;
                }

                // Check if current index_start to index_end is in dictionary
                int is_equal = 1;

                // Check if have same length
                if (index_end - index_start == code.index_end - code.index_start) {

                    // Check that each symbol matches
                    for (uint64_t k = 0; k <= index_end - index_start; k++) {
                        if (block[i + k] != block[code.index_start + k]) {
                            is_equal = 0;
                        }
                    }
                } else {
                    is_equal = 0;
                }

                if (is_equal == 1) {
                    break;
                }
            }

            // If symbols were found in the dictionary, we have to increment index end
            index_end++;
        }
    }
}