#include <stdint.h>
#include <stdio.h>

#define SYMBOLS 258


 


void huffman_encode(uint16_t *arr, uint32_t arr_length, FILE *fp) {
    // Count frequencies
    uint32_t *frequencies[SYMBOLS];
    for (uint32_t i = 0; i < arr_length; i++) {
        frequencies[arr[i]]++;
    }

    struct Element root = huffman_tree(frequencies);
}

struct Element huffman_tree(uint32_t frequencies) {
    for (int i = 0; i < SYMBOLS; i++) {

    }
}