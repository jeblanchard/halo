#pragma once

#define MAX_MEM_ADDR_32_BIT 0xffffffff

typedef enum {
    AVAILABLE_RAM = 1,
    ACPI_INFO = 3,
    RESERVED_MEMORY = 4,
    DEFECTIVE_RAM_MODULES = 5
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

#pragma pack(push, 1)
typedef struct boot_info {
    unsigned int num_kb_in_mem;
    mem_map_entry* mem_map_entry_list_base_addr;
    unsigned int mem_map_num_entries;
} boot_info;
#pragma pack(pop)

void config_phys_mem(struct boot_info* boot_info);

typedef enum alloc_block_status {
    BLOCK_ALLOC_SUCCESS = 0, 
    NO_FREE_BLOCKS = 1,
    COULD_NOT_GET_FREE_FRAME = 2,
    COULD_NOT_ALLOC_SPEC_FRAME = 3
} alloc_block_status;

#define BYTES_PER_MEMORY_BLOCK 4096

typedef struct alloc_block_resp {
    alloc_block_status status;
    unsigned int buffer_size;
    void* buffer;    
} alloc_block_resp;

alloc_block_resp alloc_block();

typedef unsigned int physical_address;

void free_block(physical_address block_address);

unsigned int get_num_blocks_in_use();

typedef enum alloc_spec_frame_status {
    ALLOC_SPEC_FRAME_SUCCESS = 0,
    ALLOC_SPEC_FRAME_BASE_DNE = 1,
    ALLOC_SPEC_FRAME_IN_USE = 2
} alloc_spec_frame_status;

typedef struct alloc_spec_frame_resp {
    alloc_spec_frame_status status;
    unsigned int buffer_size;
    void* buffer;
} alloc_spec_frame_resp;

alloc_spec_frame_resp alloc_spec_frame(physical_address frame);

void clear_phys_mem_config();

unsigned int get_num_blocks_in_use();

unsigned int get_num_free_blocks();
