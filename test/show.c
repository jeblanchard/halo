#include "stdio.h"

void show_test_passed(char* test_name) {
    printf(test_name)

    char[] msg = " passed.\n"
    printf(msg);
}

void show_test_failed(char* test_name) {
    printf(test_name);

    char[] msg = " failed.\n";
    printf(msg);
}