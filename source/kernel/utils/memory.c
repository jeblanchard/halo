void memory_copy(char* source, char* dest, int num_bytes) {
    int i;
    for (i = 0; i < num_bytes; i++) {
        *(dest + i) = *(source + i);
    }
}

void set_memory(char* base_address, unsigned char value, int length) {
    int i;
    for (i = 0; i < length; i++) {
        *(base_address + i) = value;
    }
}
