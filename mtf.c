#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define DICTIONARY_SIZE 256

void mtf_encode(uint8_t *arr, int32_t arr_length) {
    uint8_t dictionary[DICTIONARY_SIZE];
    for (int i = 0; i < DICTIONARY_SIZE; i++) {
        dictionary[i] = i;
    }

    int index;
    uint8_t c;
    for (int i = 0; i < arr_length; i++) {
        c = arr[i];
        
        // Search dictionary for item
        for (int j = 0; j < DICTIONARY_SIZE; j++) {
            if (c == dictionary[j]) {
                index = j;
                break;
            }
        }

        // Replace with arr value
        arr[i] = index;

        // Move the value to the front of the list
        for (int j = index; j > 0; j--) {
            dictionary[j] = dictionary[j - 1];
        }
        dictionary[0] = c;
    }
}

void mtf_decode(uint8_t *arr, int32_t arr_length) {
    uint8_t dictionary[DICTIONARY_SIZE];
    for (int i = 0; i < DICTIONARY_SIZE; i++) {
        dictionary[i] = i;
    }

    uint8_t c;
    int index;
    for (int i = 0; i < arr_length; i++) {
        index = arr[i];

        // Get value
        c = dictionary[index];
        arr[i] = c;

        // Move array forward
        for (int j = index; j > 0; j--) {
            dictionary[j] = dictionary[j - 1];
        }
        dictionary[0] = c;
    }
}