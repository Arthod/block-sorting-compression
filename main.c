#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <libsais64.h>

#define BLOCK_SIZE_MAX 500000000
                    // 2147483647

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))


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

        // 

        // print output for debug
        //for (int j = 0; j < block_size; j++) {
        //    printf("%c", buffer[j]);
        //}
    }

    fclose(f);

    return 0;
}