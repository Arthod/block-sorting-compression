#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#define DICTIONARY_SIZE 65536
#define RLE true

static uint16_t runA = 0;
static uint16_t runB = 1;

void mtf_encode(uint16_t *arr, int32_t arr_length) {
    int offset = 0;
    uint16_t dictionary[DICTIONARY_SIZE];
    for (int i = 0; i < DICTIONARY_SIZE; i++) {
        dictionary[i] = i;
    }

    int index;
    uint16_t c;
    for (int i = 0; i < arr_length; i++) {
        c = arr[i];
        
        // Search dictionary for item
        for (int j = 0; j < DICTIONARY_SIZE; j++) {
            if (c == dictionary[j]) {
                index = j;
                break;
            }
        }

        // Run length encode 0
        if (index == 0 && RLE) {
            uint32_t k = 1;
            while (i + k < arr_length && c == arr[i + k]) {
                k++;
            }
            uint32_t k_old = k;
            k = k - 1;
            
            bool seen_one = false;
            uint32_t bits_written = 0;
            for (int j = 0; j < 32; j++) {
                if (k & 0x01) {
                    // Bit is 1
                    seen_one = true;
                    arr[i + j - offset] = runB;
                    bits_written++;

                } else {
                    // Bit is 0
                    if (seen_one) {
                        arr[i + j - offset] = runA;
                        bits_written++;
                    }
                }
                k = k >> 1;
            }
        
            offset += k_old - bits_written;
            i += k_old - 1;
            
        } else {
            // Replace with arr value
            arr[i - offset] = index + 1;

            // Move the value to the front of the list
            for (int j = index; j > 0; j--) {
                dictionary[j] = dictionary[j - 1];
            }
            dictionary[0] = c;
        }
    }
}

void mtf_decode(uint16_t *arr, int32_t arr_length) {
    int offset = 0;
    uint16_t dictionary[DICTIONARY_SIZE];
    for (int i = 0; i < DICTIONARY_SIZE; i++) {
        dictionary[i] = i;
    }

    uint16_t c;
    int index;
    for (int i = 0; i < arr_length; i++) {
        printf("%d\n", i);
        index = arr[i];

        if ((index == runA || index == runB) && RLE) {
            printf("Here1\n");
            int s = 0;
            if (index == runB) {
                s = 1;
            }
            printf("Here2\n");

            int k = 1;
            while (i + k < arr_length && ((index = arr[i + k]) == runA || index == runB)) {
                s = s << 1;
                if (index == runB) {
                    s = s + 1;
                }
                k++;
            }

            printf("Here3\n");
            for (int j = 0; j < s + 1; j++) {
                arr[i + offset] = dictionary[0];
                offset++;
            }
            printf("Here4\n");
            i += k - 1;
        } else {
            index = index - 1;

            // Get value
            c = dictionary[index];
            arr[i + offset] = c;

            // Move array forward
            for (int j = index; j > 0; j--) {
                dictionary[j] = dictionary[j - 1];
            }
            dictionary[0] = c;
        }
    }
}