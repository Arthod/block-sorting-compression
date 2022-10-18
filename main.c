#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "compress.c"

#define BLOCK_SIZE_MAX 600000000//2000000000//500000000 * 4
                    // 2147483647

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))


int runs_count(char *arr, int arr_length) {
    int count = 0;
    for (int i = 1; i < arr_length; i++) {
        if (arr[i] != arr[i - 1]) {
            count++;
        }
    }
    
    return count;
}


int main(int argc, char** argv) {
    printf("compressor\n");

    if (argc < 2) {
        printf("include file to compress as secondary argument\n");
    }

    FILE *f = fopen(argv[1], "r");

    // Get file size
    fseek(f, 0, SEEK_END);
    float f_size = ftell(f);
    fseek(f, 0, SEEK_SET);

    // Compress for each block size separatly
    int blocks_count = ceil(f_size / BLOCK_SIZE_MAX);
    for (int i = 0; i < blocks_count; i++) {
        int block_size = min(BLOCK_SIZE_MAX, f_size - BLOCK_SIZE_MAX * i);
        char *buffer = malloc(block_size * sizeof(char));
        fgets(buffer, block_size + 1, f);
        printf("Compressing block %d of %d with block_size=%d\n", i + 1, blocks_count, block_size);

        int64_t bwt_primary_index = bwt_transform(buffer, block_size);
        printf("Run length %d\n", runs_count(buffer, block_size));

        // Write transformed to out
        FILE *f_out = fopen("out.txt", "w");
        for (int j = 0; j < block_size; j++) {
            fprintf(f_out, "%c", buffer[j]);
        }
        fclose(f_out);
    }

    fclose(f);

    return 0;
}
