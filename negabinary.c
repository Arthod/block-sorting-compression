#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

void dec_to_negabinary(int32_t dec, int8_t *negabinary, size_t size) {
    size_t index = 0;
    while (dec != 0) {
        int remainder = dec % -2;
        dec = dec / -2;

        // If the remainder is negative, adjust it and increment the quotient
        if (remainder < 0) {
            remainder += 2;
            dec += 1;
        }

        if (index < size) {
            negabinary[index++] = remainder;
        } else {
            printf("Negabinary buffer is too small.\n");
            return;
        }
    }

    // Add a leading 0 if the input was 0
    if (index == 0 && size > 0) {
        negabinary[index++] = 0;
    }

    // Add null terminator
    if (index < size) {
        negabinary[index] = -1;
    } else {
        printf("Negabinary buffer is too small.\n");
    }
}

int main() {
    int32_t decimal_number;
    printf("Enter a decimal number: ");
    scanf("%d", &decimal_number);

    int8_t negabinary[128];

    dec_to_negabinary(decimal_number, negabinary, sizeof(negabinary));

    printf("Negabinary representation: ");
    for (size_t i = 0; negabinary[i] != -1; ++i) {
        printf("%d", negabinary[i]);
    }
    printf("\n");

    return 0;
}