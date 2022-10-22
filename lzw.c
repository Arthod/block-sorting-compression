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
    uint16_t dictionary_size = 0;
    for (int i = 0; i < 65536; i++) {
        Code *code = malloc(sizeof(Code));
        code->index_start = 0;
        code->index_end = 0;
        code->symbol_value = 0;
        dictionary[i] = *code;
    }
    /*for (int i = 0; i < 256; i++) {
        Code *code = malloc(sizeof(Code));
        code->index_start = NULL;
        code->index_end = NULL;
        dictionary[i] = *code;
    }*/
    
    uint16_t *block_new = malloc(block_size * sizeof(uint16_t));
    uint64_t block_size_new = 0;

    for (uint64_t i = 0; i < block_size; i++) {
        uint64_t index_start = i;
        uint64_t index_end = i + 1;

        /*// Check if two-characters interval is in dictionary, if not, add it
        Code code = dictionary[block[i]];
        if (code.symbol_value != block[i]) {
            code.index_start = i;
            code.index_end = i;
            code.symbol_value = block[i];
        }*/

        // Increment index_end until symbols are not in dictionary
        while (1) {
            int is_in_dictionary = 0;
            // Check if symbols from index_start to index_end are in dictionary
            for (uint16_t j = 0; j < dictionary_size; j++) {
                printf("Looping through dictionary, j = %d\n", j);
                Code code = dictionary[j];

                // Check if have same length
                if (index_end - index_start == code.index_end - code.index_start) {
                    printf("Has same length\n");

                    // Check that each symbol matches
                    uint16_t match = 1;
                    for (uint64_t k = 0; k <= index_end - index_start; k++) {
                        if (block[i + k] != block[code.index_start + k]) {
                            match = 0;
                            break;
                        }
                    }
                    if (match == 0) {
                        printf("Symbols dont match\n");
                        continue;
                    }
                } else {
                    printf("Difference in length: %ld vs %ld\n", index_end - index_start, code.index_end - code.index_start);
                    continue;
                }

                is_in_dictionary = 1;
                //printf("No difference found between symbol and item in dictionary\n");
                break;
            }

            // If symbols were found in the dictionary, we have to increment index end
            if (is_in_dictionary == 1) {
                printf("Found in dictionary symbol: '");
                for (uint64_t k = 0; k <= index_end - index_start; k++) {
                    printf("%c", block[i + k]);
                }
                printf("'\n");
                //printf("Incrementing index_end\n");
                index_end++;
            } else {
                // Add to dictionary
                dictionary[dictionary_size].index_end = index_end;
                dictionary[dictionary_size].index_start = index_start;
                dictionary[dictionary_size].symbol_value = dictionary_size + 256;
                
                printf("Added new to dictionary symbol: '");
                for (uint64_t k = 0; k <= index_end - index_start; k++) {
                    printf("%c", block[i + k]);
                }
                printf("' with symbol value: %d, and index interval: %ld-%ld\n", dictionary_size + 256, index_start, index_end);

                dictionary_size++;
                break;
            }
        }
    }
}