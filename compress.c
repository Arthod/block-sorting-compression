#include <stdio.h>
#include <stdlib.h>
#include "libsais64.h"

int64_t bwt_transform(char *block, int block_size) {
    int fs = 0;
    uint64_t *temp = malloc((block_size - 1 + fs) * sizeof(uint64_t));
    int64_t primary_index = libsais64_bwt(block, block, temp, block_size, fs, NULL);

}

