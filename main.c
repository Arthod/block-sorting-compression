#include <stdio.h>
#include <math.h>

#define BLOCK_SIZE_MAX 5000000//500000000

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))


int main(int argc, char** argv) {
    //printf("include file to compress as secondary argument");

    if (argc < 2) {
        printf("include file to compress as secondary argument");
    }

    FILE *f = fopen(argv[1], "r");

    // Get file size
    //fseek(f, 0, SEEK_END);
    //float f_size = ftell(f);
    //fseek(f, 0, SEEK_SET);

    // Compress for each block size separatly
    int blocks_count = 1;//ceil(f_size / BLOCK_SIZE_MAX);
    for (int i = 0; i < blocks_count; i++) {
        //int block_size = min(BLOCK_SIZE_MAX, f_size - BLOCK_SIZE_MAX * i);
        //char buffer[block_size];
        char *buffer = malloc(BLOCK_SIZE_MAX * sizeof(char));
        
        fgets(buffer, BLOCK_SIZE_MAX - 1, f);

    }

    fclose(f);

    return 0;
}