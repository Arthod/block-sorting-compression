#include <stdio.h>

int main() {
    int s = 0;
    s << 1;
    s = (s << 1) + 1;
    s = s << 1 << 1;
    printf("%d\n", s);
    return 0;
}