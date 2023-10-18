// Copy bytes from one place to another.
void memory_copy(unsigned char * source, unsigned char * dest, int num_bytes);

// Set a series of bytes in memory to a single value.
void set_memory(void* base_address, unsigned char value, int length);

void * allocate(unsigned int num_bytes);

void * clear(void * ptr_to_buffer);
