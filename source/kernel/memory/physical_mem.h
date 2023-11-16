#include "boot.h"

void init_phys_mem(struct boot_info * boot_info);

typedef enum block_alloc_stat {
    BLOCK_ALLOC_SUCCESS = 0, 
    NO_FREE_BLOCKS = 1
} block_alloc_stat;

#define BYTES_PER_MEMORY_BLOCK 4096

typedef struct block_alloc_resp {
    block_alloc_stat status;
    int buffer_size;
    void* buffer;
} block_alloc_resp;

block_alloc_resp alloc_block();

void free_block();

typedef unsigned int physical_address;

unsigned int get_num_blocks_in_use();
