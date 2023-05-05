#include <stdint.h>
#include <stdio.h>

#define SYMBOLS 258


void huffman_encode(uint16_t *arr, uint32_t arr_length, FILE *fp) {
    // Count frequencies
    uint32_t *frequencies[SYMBOLS];
    for (uint32_t i = 0; i < arr_length; i++) {
        frequencies[arr[i]]++;
    }


}