void memory_copy(char* source, char* dest, int num_bytes) {
    int i;
    for (i = 0; i < num_bytes; i++) {
        *(dest + i) = *(source + i);
    }
}
