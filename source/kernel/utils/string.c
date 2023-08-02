#include "../drivers/screen.h"

char* int_to_string(unsigned int val, unsigned int base) {
    static char buf[32] = {0};
    for (int i = 0; i <= 30; i++) {
        buf[i] = '0';
    }

    if (val == 0) {
        return &buf[30];
    }

    int i = 30;
    char digits[] = "0123456789abcdef";
    for(; val != 0 && i >= 0; i--, val /= base) {
        int index = val % base;
        buf[i] = digits[index];
    }

    return &buf[i+1];
}
