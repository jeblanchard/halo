#include "boot.h"

void init_phys_mem(struct boot_info * boot_info);

typedef enum block_alloc_stat {
    BLOCK_ALLOC_SUCCESS = 0, 
    NO_FREE_BLOCKS = 1
} block_alloc_stat;

typedef struct block_alloc_resp {
    block_alloc_stat status;
    void* buffer;
} block_alloc_resp;

block_alloc_resp alloc_block();
