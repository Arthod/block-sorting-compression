#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define DICTIONARY_SIZE 258
#define RLE true

static uint16_t runA = 0;
static uint16_t runB = 1;
static uint16_t EOFF = 257;

uint32_t mtf_encode(uint8_t *arr_in, uint32_t arr_in_length, uint16_t *arr_out) {
    uint8_t dictionary[DICTIONARY_SIZE];
    for (int i = 0; i < DICTIONARY_SIZE; i++) {
        dictionary[i] = i;
    }

    int index;
    uint16_t c;
    int offset = 0;
    for (int i = 0; i < arr_in_length; i++) {
        c = arr_in[i];
        
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
            while (i + k < arr_in_length && c == arr_in[i + k]) {
                k++;
            }
            
            uint32_t bits_required = floor(log2(k + 1));
            uint32_t k_other = k + 1 - (1 << bits_required);
            //printf("k = %d, k2 = %d: ", k, kkk);
            for (int j = 0; j < bits_required; j++) {
                if ((k_other >> j) & 1) {
                    // Bit is 1
                    arr_out[i + j - offset] = runB;
                    //printf("%d", runB);

                } else {
                    // Bit is 0
                    arr_out[i + j - offset] = runA;
                    //printf("%d", runA);
                }
            }
            //printf("\n");
        
            offset += k - bits_required;
            i += k - 1;
            
        } else {
            // Replace with arr value
            arr_out[i - offset] = index + 1;

            // Move the value to the front of the list
            for (int j = index; j > 0; j--) {
                dictionary[j] = dictionary[j - 1];
            }
            dictionary[0] = c;
        }
    }

    if (offset == 0) {
        printf("ERROR.. Nothing was compressed.. can't add EOF");
        return 0;
    }
    arr_out[arr_in_length - offset] = EOFF;

    return arr_in_length - offset + 1;
}

uint32_t mtf_decode(uint16_t *arr_in, uint32_t arr_in_length, uint8_t *arr_out) {
    int offset = 0;
    uint8_t dictionary[DICTIONARY_SIZE];
    for (int i = 0; i < DICTIONARY_SIZE; i++) {
        dictionary[i] = i;
    }

    uint8_t c;
    int index;
    for (int i = 0; i < arr_in_length; i++) {
        index = arr_in[i];

        if ((index == runA || index == runB) && RLE) {
            int s = 0;
            if (index == runB) {
                s = 1;
            }

            int k = 1;
            while (i + k < arr_in_length && ((index = arr_in[i + k]) == runA || index == runB)) {
                if (index == runB) {
                    s = s + (1 << k);
                }
                k++;
            }
            s = s + (1 << k) - 1;

            for (int j = 0; j < s; j++) {
                arr_out[i + offset + j] = dictionary[0];
            }
            offset += s - k;
            i += k - 1;
        } else {
            index = index - 1;

            // Get value
            c = dictionary[index];
            arr_out[i + offset] = c;

            // Move array forward
            for (int j = index; j > 0; j--) {
                dictionary[j] = dictionary[j - 1];
            }
            dictionary[0] = c;
        }
    }

    return arr_in_length + offset - 1;
}