#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>

#define DICTIONARY_SIZE 256
#define RLE true

static uint16_t runA = 0;
static uint16_t runB = 1;

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
            uint32_t k_other = k - 1;
            
            bool seen_one = false;
            uint32_t bits_written = 0;
            if (k_other == 0) {
                arr_out[i - offset] = runA;
                bits_written++;
            } else {
                for (int j = 0; j < ceil(log2(k_other)); j++) {
                    if (k_other & 0x01) {
                        // Bit is 1
                        seen_one = true;
                        arr_out[i + j - offset] = runB;
                        bits_written++;

                    } else {
                        // Bit is 0
                        if (seen_one) {
                            arr_out[i + j - offset] = runA;
                            bits_written++;
                        }
                    }
                    k_other = k_other >> 1;
                }
            }
        
            offset += k - bits_written;
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

    uint32_t arr_out_length = arr_in_length - offset;
    return arr_out_length;
}

uint32_t mtf_decode(uint16_t *arr_in, uint32_t arr_in_length, uint8_t arr_out) {
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
                s = s << 1;
                if (index == runB) {
                    s = s + 1;
                }
                k++;
            }

            for (int j = 0; j < s; j++) {
                arr_in[i + offset] = dictionary[0];
                offset++;
            }
            i += k - 1;
        } else {
            index = index - 1;

            // Get value
            c = dictionary[index];
            arr_in[i + offset] = c;

            // Move array forward
            for (int j = index; j > 0; j--) {
                dictionary[j] = dictionary[j - 1];
            }
            dictionary[0] = c;
        }
    }

    
}