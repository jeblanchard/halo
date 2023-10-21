#include "stdlib.h"

void memory_copy(unsigned char * source, unsigned char * dest, int num_bytes) {
    int i;
    for (i = 0; i < num_bytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void set_memory(unsigned char * base_address, unsigned char value, int length) {
    int i;
    for (i = 0; i < length; i++) {
        *(base_address + i) = value;
    }
}

void * allocate(unsigned int num_bytes) {
    num_bytes += 1;
    return NULL;
}

void clear(void * ptr_to_buffer) {
    ptr_to_buffer += 1;
}

