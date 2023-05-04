#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "bwt.c"
#include "compress.c"
#include "frequency_suffix_tree.c"
#include "lzw.c"
#include "mtfrle.c"
#include <stdint.h>

#define BLOCK_SIZE_MAX 2000000000//2000000000//500000000 * 4
                    // 2147483647

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))


int runs_count(uint8_t *arr, int arr_length) {
    int count = 0;
    for (int i = 1; i < arr_length; i++) {
        if (arr[i] != arr[i - 1])
            count++;
    }
    
    return count;
}

void print_arr(uint16_t *block, uint64_t block_size) {
    for (int i = 0; i < block_size; i++) {
        printf("%d, ", block[i]);
    }
    printf("\n");
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
        uint8_t *block = malloc(block_size * sizeof(uint8_t));
        uint8_t *block_saved = malloc(block_size * sizeof(uint8_t));
        uint32_t runs;
        float run_length;
        
        printf("Compressing block %d of %d with block_size: %d\n", i + 1, blocks_count, block_size);

        // Read file upto block size
        for (int j = 0; j < block_size; j++) {
            block[j] = fgetc(f);
            block_saved[j] = block[j];
        }

        // Print occurrences
        //print_occurrences(block, block_size);
        //lzw_encode(block, block_size);
        //fst_create(block, block_size, 3);

        
        // Compute and print total number of runs before BWT
        runs = runs_count(block, block_size);
        run_length = block_size / (float) runs;
        printf("Runs count before %d with average run length %f\n", runs, run_length);
        
        // Compute and print total number of runs after BWT
        //int64_t bwt_primary_index = bwt_transform(block, block_size, alphabet);
        runs = runs_count(block, block_size);
        run_length = block_size / (float) runs;
        printf("Runs count after %d with average run length %f\n", runs, run_length);

        // Move to front
        printf("MTF encoding.\n");
        uint16_t *arr_mtf_encoded = malloc(block_size * sizeof(uint16_t));
        uint32_t arr_mtf_encoded_length = mtf_encode(block, block_size, arr_mtf_encoded);
        printf("%d\n", arr_mtf_encoded_length);

        print_arr(arr_mtf_encoded, arr_mtf_encoded_length);

        // Move to front decode
        printf("MTF decoding.\n");
        int block_size2 = mtf_decode(arr_mtf_encoded, arr_mtf_encoded_length, block);

        // Check lengths
        if (block_size != block_size2) {
            printf("Not same length.\n");
            return -1;
        }

        // Reverse BWT and verify it is the same as input file
        printf("BWT reverse transform.\n");
        //bwt_reverse_transform(block, block_size2, bwt_primary_index, alphabet);
        for (int j = 0; j < block_size2; j++) {
            if (block_saved[j] != block[j]) {
                printf("Error occured. BWT reverse and BWT input is not the same\n");
                return -1;
            }
        }
        printf("Compression and decompression was successful\n");

        free(arr_mtf_encoded);
        free(block);
        free(block_saved);
    }

    free(alphabet);
    fclose(f);

    return 0;
}
