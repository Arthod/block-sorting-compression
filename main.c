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

void print_arr16(uint16_t *arr, uint64_t arr_length) {
    for (int i = 0; i < arr_length; i++) {
        printf("%d, ", arr[i]);
    }
    printf("\n");
}
void print_arr8(uint8_t *arr, uint64_t arr_length) {
    for (int i = 0; i < arr_length; i++) {
        printf("%d, ", arr[i]);
    }
    printf("\n");
}

void print_occurrences(uint8_t *arr, uint64_t arr_length) {
    uint64_t *count = malloc(256 * sizeof(uint64_t)); 

    for (int i = 0; i < arr_length; i++) {
        count[arr[i]]++;
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
    FILE *f_out = fopen("compressed.fzip", "w+");
    FILE *f_in = fopen(argv[1], "r");
    int f_size = file_size(f_in);

    // Compress for each block size separatly
    int blocks_count = ceil((float) f_size / BLOCK_SIZE_MAX);
    printf("File size: %d, maximum block size: %d, number of blocks: %d\n", f_size, BLOCK_SIZE_MAX, blocks_count);
    for (int i = 0; i < blocks_count; i++) {
        uint32_t block8_length = min(BLOCK_SIZE_MAX, f_size - BLOCK_SIZE_MAX * i);
        uint32_t block16_length = 0;
        uint32_t block8_length_old = block8_length;

        uint8_t *block8 = malloc(block8_length * sizeof(uint8_t));
        uint16_t *block16 = malloc(block8_length * sizeof(uint16_t));
        uint8_t *block8_old = malloc(block8_length * sizeof(uint8_t));

        uint32_t runs;
        float run_length;
        
        printf("Compressing block %d of %d with block length: %d\n", i + 1, blocks_count, block8_length);

        // Read file upto block size
        for (int j = 0; j < block8_length; j++) {
            block8[j] = fgetc(f_in);
            block8_old[j] = block8[j];
        }

        // Compute and print total number of runs before BWT
        runs = runs_count(block8, block8_length);
        run_length = block8_length / (float) runs;
        printf("Runs count before %d with average run length %f\n", runs, run_length);
        
        // Compute and print total number of runs after BWT
        int64_t bwt_primary_index = bwt_transform(block8, block8_length, alphabet);
        runs = runs_count(block8, block8_length);
        run_length = block8_length / (float) runs;
        printf("Runs count after %d with average run length %f\n", runs, run_length);

        // Move to front
        printf("MTF and RLE encoding.\n");
        block16_length = mtf_encode(block8, block8_length, block16);
        printf("%d\n", block16_length);

        // Huffman encode
        //uint32_t encoded_length = huffman_encode()

        // WRITE TO FILE
        //fwrite()


        // Move to front decode
        printf("MTF and RLE decoding.\n");
        block8_length = mtf_decode(block16, block16_length, block8);

        //print_arr8(block, block_size2);

        // Check lengths
        if (block8_length_old != block8_length) {
            printf("Not same length. OLD=%d vs NEW=%d\n", block8_length_old, block8_length);
            return -1;
        }

        // Reverse BWT and verify it is the same as input file
        printf("BWT reverse transform.\n");
        bwt_reverse_transform(block8, block8_length, bwt_primary_index, alphabet);
        for (int j = 0; j < block8_length; j++) {
            if (block8[j] != block8_old[j]) {
                printf("Error occured. BWT reverse and BWT input is not the same\n");
                return -1;
            }
        }
        printf("Compression and decompression was successful\n");

    
        free(block8);
        free(block16);
        free(block8_old);
    }

    free(alphabet);
    fclose(f_in);
    fclose(f_out);

    return 0;
}
