#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "compress.c"

#define BLOCK_SIZE_MAX 2000000000//2000000000//500000000 * 4
                    // 2147483647

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))


int runs_count(char *arr, int arr_length) {
    int count = 0;
    for (int i = 1; i < arr_length; i++) {
        if (arr[i] != arr[i - 1])
            count++;
    }
    
    return count;
}


int main(int argc, char** argv) {
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
        uint8_t *block = malloc(block_size * sizeof(uint8_t));

        for (int j = 0; j < block_size; j++) {
            block[j] = fgetc(f);
        }
        //fgets(block, block_size + 1, f);
        /*for (int j = 0; j < block_size; j++) {
            printf("%c", block[j]);
        }*/

        int runs = runs_count(block, block_size);
        printf("Runs count before %d with average run length %f\n", runs, block_size / (float) runs);

        printf("Compressing block %d of %d with block_size=%d\n", i + 1, blocks_count, block_size);
        
        int64_t bwt_primary_index = bwt_transform(block, block_size);
        runs = runs_count(block, block_size);
        printf("Runs count after %d with average run length %f\n", runs, block_size / (float) runs);

        bwt_reverse_transform_(block, block_size, bwt_primary_index);
        runs = runs_count(block, block_size);
        printf("Runs count after %d with average run length %f\n", runs, block_size / (float) runs);

        // Write transformed to out
        FILE *f_out = fopen("out.txt", "w");
        for (int j = 0; j < block_size; j++) {
            fprintf(f_out, "%c", block[j]);
        }
        fclose(f_out);

        free(block);
    }

    fclose(f);

    return 0;
}
