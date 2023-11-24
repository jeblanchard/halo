#pragma once

typedef enum {
    AVAILABLE_RAM = (unsigned int) 1,
    ACPI_INFO = (unsigned int) 3,
    RESERVED_MEMORY = (unsigned int) 4,
    DEFECTIVE_RAM_MODULES = (unsigned int) 5
} mem_range_type;

#pragma pack(push, 1)
typedef struct mem_map_entry {
    unsigned int base_addr_low;

    // On a 32-bit system we
    // will never use this.
    unsigned int base_addr_high;

    unsigned int length_in_bytes_low;

    // On a 32-bit system we
    // will never use this.
    unsigned int length_in_bytes_high;

    mem_range_type type;
} mem_map_entry;
#pragma pack(pop)

// A struct representing what the boot sector provides
// the kernel upon entry.
#pragma pack(push, 1)
typedef struct boot_info {
    unsigned int num_kb_in_mem;
    mem_map_entry* mem_map_entry_list_base_addr;
    unsigned int mem_map_num_entries;
} boot_info;
#pragma pack(pop)

void init_phys_mem(struct boot_info* boot_info);

typedef enum block_alloc_stat {
    BLOCK_ALLOC_SUCCESS = 0, 
    NO_FREE_BLOCKS = 1
} block_alloc_stat;

#define BYTES_PER_MEMORY_BLOCK 4096

typedef struct block_alloc_resp {
    block_alloc_stat status;
    unsigned int buffer_size;
    void* buffer;    
} block_alloc_resp;

block_alloc_resp alloc_block();

typedef unsigned int physical_address;

void free_block(physical_address block_address);

unsigned int get_num_blocks_in_use();
