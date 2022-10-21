#include <stdio.h>
#include <stdlib.h>
#include "libsais64.h"
#include <time.h>

#define UNUSED_BYTE -1
#define ALPHABET_SIZE 256

void swap(uint64_t *arr, uint64_t idx1, uint64_t idx2) {
    uint64_t temp = arr[idx1];
    arr[idx1] = arr[idx2];
    arr[idx2] = temp;
}
uint64_t partition(uint64_t *index_array, uint8_t *compared_array, uint64_t start_index, uint64_t end_index) {
    uint64_t pivot = index_array[end_index];
    uint64_t i = start_index - 1;

    for (uint64_t j = start_index; j < end_index; j++) {
        // TODO: can optimize.. We know first two characters of the word are already sorted, no need to recheck them
        uint64_t k = 0;

        while (1) {
            if (compared_array[index_array[j] + k] != compared_array[pivot + k]) {
                // If they are not equal, check comparison, and swap if greater than pivot
                if (compared_array[index_array[j] + k] <= compared_array[pivot + k]) {
                    i++;
                    swap(index_array, i, j);
                }
                break;
            }
            k++;
        }
    }

    swap(index_array, i + 1, end_index);
    return i + 1;
}

uint64_t randomized_partition(uint64_t *index_array, uint8_t *compared_array, uint64_t start_index, uint64_t end_index) {
    uint64_t random_index = (rand() % (end_index - start_index + 1)) + start_index;
    if (random_index > end_index || random_index < start_index) {
        printf("Randomized partition error\n");
    }
    swap(index_array, end_index, random_index);
    return partition(index_array, compared_array, start_index, end_index);
}
void randomized_quicksort_index_array(uint64_t *index_array, uint8_t *compared_array, uint64_t start_index, uint64_t end_index) {
    // In-place implementation of quicksort that sorts an index 
    // array based on the comparable values of a comparable array
    if (start_index >= end_index) {
        return;
    }
    
    int q = randomized_partition(index_array, compared_array, start_index, end_index);
    randomized_quicksort_index_array(index_array, compared_array, start_index, q - 1);
    randomized_quicksort_index_array(index_array, compared_array, q + 1, end_index);
}



int64_t bwt_transform(uint8_t *block, int32_t block_size, uint8_t *alphabet) {
    // Apply alphabet if not NULL
    if (alphabet != NULL) {
        for (int i = 0; i < block_size; i++) {
            block[i] = alphabet[block[i]];
        }
    }

    int32_t fs = 0;
    int64_t *temp = malloc((block_size - 1 + fs) * sizeof(int64_t));
    int64_t primary_index = libsais64_bwt(block, block, temp, block_size, fs, NULL);

    free(temp);

    return primary_index;
}

int32_t bwt_reverse_transform(uint8_t *block, int32_t block_size, int64_t primary_index, uint8_t *alphabet) {
    int64_t *temp = malloc((block_size + 1) * sizeof(int64_t));
    
    int32_t err = libsais64_unbwt(block, block, temp, block_size, NULL, primary_index);

    free(temp);

    // Reverse apply alphabet if not NULL
    if (alphabet != NULL) {
        // Flip index/value
        uint8_t alphabet_reverse[ALPHABET_SIZE];
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            alphabet_reverse[alphabet[i]] = i;
        }

        for (int i = 0; i < block_size; i++) {
            block[i] = alphabet_reverse[block[i]];
        }
    }

    return err;
}
int64_t bwt_transform_mine(uint8_t *block, int32_t block_size) {
    srand(time(NULL));
    /// Compute the BWT using radix sort on the first two characters, then
    /// quicksort on the rest. Burrows-Wheeler method for computation of SA.

    int16_t *block_temp = malloc((block_size + 1) * sizeof(int16_t));
    uint64_t *V_temp = malloc((block_size) * sizeof(uint64_t));
    uint64_t *V = malloc((block_size) * sizeof(uint64_t));

    uint64_t *count = malloc(257 * sizeof(uint64_t));
    uint64_t *count_current = malloc(257 * sizeof(uint64_t));

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
        uint16_t k = block_temp[i + 1] + 1;
        V_temp[count_current[k] - 1] = i;
        count_current[k]--;
    }
    free(count_current);

    // Sort by first character
    count[0]--;
    count[block_temp[0] + 1]++;
    for (int i = 1; i < 257; i++) {
        count[i] += count[i - 1];
    }

    // Go in reverse, and write the index of the index
    for (int i = block_size - 1; i >= 0; i--) {
        uint16_t k = block_temp[V_temp[i]] + 1;
        V[count[k] - 1] = V_temp[i];
        count[k]--;
    }
    free(count);
    free(V_temp);

    /*
    // Test V array is sorted by first two characters
    for (int i = 0; i < block_size - 2; i++) {
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
    }*/

    /// Quicksort Q5
    uint64_t amount_compared_equal_total = 0;
    uint64_t first = 0;
    for (int ch1 = 0; ch1 < 256; ch1++) {
        // Q6
        for (int ch2 = -1; ch2 < 256 && amount_compared_equal_total < block_size; ch2++) {
            first = amount_compared_equal_total;

            while (ch1 == block_temp[V[amount_compared_equal_total]] && ch2 == block_temp[V[amount_compared_equal_total] + 1]) {
                amount_compared_equal_total++;

                // If reached end of input, stop all
                if (amount_compared_equal_total == block_size) {
                    break;
                }
            }

            // If there's atleast two that are compared equal we need to sort them.
            if (amount_compared_equal_total - first >= 2) {
                randomized_quicksort_index_array(V, block, first, amount_compared_equal_total - 1);
            }
        }
    }


    // Test V array is sorted
    for (uint64_t i = 0; i < block_size - 2; i++) {
        uint64_t c = V[i];
        uint64_t l = V[i + 1];

        while (block_temp[c] == block_temp[l]) {
            c++;
            l++;
        }
        if (block_temp[c] > block_temp[l]) {
            printf("Error test 2\n");
        }
    }
    free(block_temp);

    // Now our assumption is that V is sorted correctly.
    // From the suffix array we can form bwt(s)
    uint64_t primary_index = 0;
    uint8_t *out_arr = malloc(block_size * sizeof(uint8_t));
    int zero_seen = 1;
    for (uint64_t i = 0; i < block_size; i++) {
        if (V[i] == 0) {
            primary_index = i + 1;
            zero_seen = 0;
        } else {
            out_arr[i + zero_seen] = block[V[i] - 1];
        }
    }
    out_arr[0] = block[block_size - 1];
    
    for (uint64_t i = 0; i < block_size; i++) {
        block[i] = out_arr[i];
    }

    free(out_arr);
    free(V);

    return primary_index;
}
