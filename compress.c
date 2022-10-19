#include <stdio.h>
#include <stdlib.h>
#include "libsais64.h"

int64_t bwt_transform(uint8_t *block, int32_t block_size) {
    int32_t fs = 0;
    int64_t *temp = malloc((block_size - 1 + fs) * sizeof(int64_t));
    int64_t primary_index = libsais64_bwt(block, block, temp, block_size, fs, NULL);

    free(temp);

    return primary_index;
}

int32_t bwt_reverse_transform(uint8_t *block, int32_t block_size, int64_t primary_index) {
    int64_t *temp = malloc((block_size + 1) * sizeof(int64_t));
    
    int32_t err = libsais64_unbwt(block, block, temp, block_size, NULL, primary_index);

    free(temp);

    return err;
}