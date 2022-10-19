#include <stdio.h>
#include <stdlib.h>
#include "libsais64.h"

#define UNUSED_BYTE -1

int64_t bwt_transform_(uint8_t *block, int32_t block_size) {
    int32_t fs = 0;
    int64_t *temp = malloc((block_size - 1 + fs) * sizeof(int64_t));
    int64_t primary_index = libsais64_bwt(block, block, temp, block_size, fs, NULL);

    free(temp);

    return primary_index;
}

int32_t bwt_reverse_transform_(uint8_t *block, int32_t block_size, int64_t primary_index) {
    int64_t *temp = malloc((block_size + 1) * sizeof(int64_t));
    
    int32_t err = libsais64_unbwt(block, block, temp, block_size, NULL, primary_index);

    free(temp);

    return err;
}
int64_t bwt_transform(uint8_t *block, int32_t block_size) {
    /// Compute the BWT using radix sort on the first two characters, then
    /// quicksort on the rest. Burrows-Wheeler method for computation of SA.

    int16_t *block_temp = malloc(block_size * sizeof(int16_t));
    uint64_t *count = malloc(257 * sizeof(uint64_t));
    uint64_t *count_current = malloc(257 * sizeof(uint64_t));
    uint64_t *V_temp = malloc((block_size + 1) * sizeof(uint64_t));
    uint64_t *V = malloc((block_size + 1) * sizeof(uint64_t));

    for (int i = 0; i < block_size; i++) {
        block_temp[i] = block[i];
    }
    block_temp[block_size] = UNUSED_BYTE;

    /// Radix sort by first two characters using radix sort using counting sort
    // Sort by second character
    for (int i = 1; i < block_size + 1; i++) {
        count[block_temp[i] + 1]++;
    }

    count_current[0] = 1;
    for (int i = 1; i < 257; i++) {
        count_current[i] = count[i] + count_current[i - 1];
    }

    // Go in reverse and write the index
    for (int i = block_size - 1; i >= 0; i--) {
        uint64_t k = block_temp[i + 1] + 1;
        V_temp[count_current[k] - 1] = i;
        count_current[k]--;
    }

    // Sort by first character
    count[0]--;
    count[block_temp[0] + 1]++;
    for (int i = 1; i < 257; i++) {
        count[i] += count[i - 1];
    }

    // Go in reverse, and write the index of the index
    for (int i = block_size - 1; i >= 0; i--) {
        uint64_t k = block_temp[V_temp[i]] + 1;
        V[count[k] - 1] = V_temp[i];
        count[k]--;
    }

    // Test V array is sorted by first two characters
    for (int i = 0; i < block_size; i++) {
        uint64_t c = V[i];
        uint64_t l = V[i + 1];
        if (block_temp[c] > block_temp[l]) {
            printf("Error1\n");
        }
        if (block_temp[c] == block_temp[l]) {
            if (block_temp[c + 1] > block_temp[l + 1]) {
                printf("Error2\n");
            }
        }
    }
}