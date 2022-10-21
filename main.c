#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "bwt.c"
#include "compress.c"
#include "frequency_suffix_tree.c"

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

void print_occurrences(uint8_t *block, uint64_t block_size) {
    uint64_t *count = malloc(256 * sizeof(uint64_t)); 

    for (int i = 0; i < block_size; i++) {
        count[block[i]]++;
    }

    printf("[");
    for (int i = 0; i < 256; i++) {
        printf("%ld, ", count[i]);
    }
    printf("]\n");
}

int file_size(FILE *f) {
    fseek(f, 0, SEEK_END);
    int f_size = ftell(f);
    fseek(f, 0, SEEK_SET);
    
    return f_size;
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("include file to compress as secondary argument\n");
        return -1;
    }

    // Write alphabet
    uint8_t *alphabet = malloc(256 * sizeof(uint8_t));;
    int idx = 0;
    for (int i = 96; i < 128; i++) alphabet[idx++] = i;
    for (int i = 64; i < 96; i++) alphabet[idx++] = i;
    for (int i = 32; i < 64; i++) alphabet[idx++] = i;
    for (int i = 0; i < 32; i++) alphabet[idx++] = i;
    for (int i = 128; i < 256; i++) alphabet[idx++] = i;

    
    for (int i = 0; i < 256; i++) alphabet[i] = i;

    // Get file size
    FILE *f = fopen(argv[1], "r");
    int f_size = file_size(f);

    // Compress for each block size separatly
    int blocks_count = ceil((float) f_size / BLOCK_SIZE_MAX);
    printf("File size: %d, maximum block size: %d, number of blocks: %d\n", f_size, BLOCK_SIZE_MAX, blocks_count);
    for (int i = 0; i < blocks_count; i++) {
        int block_size = min(BLOCK_SIZE_MAX, f_size - BLOCK_SIZE_MAX * i);
        uint16_t *block = malloc(block_size * sizeof(uint16_t));
        uint8_t *block_saved = malloc(block_size * sizeof(uint8_t));
        uint32_t runs;

        printf("Compressing block %d of %d with block_size: %d\n", i + 1, blocks_count, block_size);

        // Read file upto block size
        for (int j = 0; j < block_size; j++) {
            block[j] = fgetc(f);
            block_saved[j] = block[j];
        }

        // Print occurrences
        //print_occurrences(block, block_size);
        fst_create(block, block_size, 3);

        /*
        // Compute and print total number of runs before BWT
        runs = runs_count(block, block_size);
        printf("Runs count before %d with average run length %f\n", runs, block_size / (float) runs);
        
        // Compute and print total number of runs after BWT
        int64_t bwt_primary_index = bwt_transform(block, block_size, alphabet);
        runs = runs_count(block, block_size);
        printf("Runs count after %d with average run length %f\n", runs, block_size / (float) runs);

        // Write BWT of string to out file and close file
        FILE *f_out = fopen("out.txt", "w");
        for (int j = 0; j < block_size; j++) {
            fprintf(f_out, "%c", block[j]);
        }
        fclose(f_out);

        // Reverse BWT and verify it is the same as input file
        bwt_reverse_transform(block, block_size, bwt_primary_index, alphabet);
        for (int j = 0; j < block_size; j++) {
            if (block_saved[j] != block[j]) {
                printf("Error occured. BWT reverse and BWT input is not the same\n");
                return -1;
            }
        }*/

        free(block);
        free(block_saved);
    }

    free(alphabet);
    fclose(f);

    return 0;
}
