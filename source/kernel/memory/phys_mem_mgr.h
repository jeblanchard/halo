#pragma once

#include <stdbool.h>

#define MAX_MEM_ADDR_32_BIT 0xffffffff

typedef enum {
    AVAILABLE_TO_OS = 1,
    SYSTEM_RESERVED = 2,
    ACPI_RECLAIMABLE = 3,
    ACPI_NVS = 4
} mem_range_type;

#pragma pack(push, 1)
typedef struct mem_map_entry {
    unsigned int base_addr_low;
    unsigned int base_addr_high;

    unsigned int length_in_bytes_low;
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
    ALLOC_BLOCK_SUCCESS = 0, 
    ALLOC_BLOCK_ALL_BLOCKS_IN_USE = 1,
    ALLOC_BLOCK_FAILED_GET_FREE_FRAME = 2,
    ALLOC_BLOCK_FAILED_ALLOC_SPEC_FRAME = 3
} alloc_block_status;

#define BYTES_PER_MEMORY_BLOCK 4096

typedef unsigned int physical_address;

typedef struct alloc_block_resp {
    alloc_block_status status;
    unsigned int buffer_size;
    physical_address buffer_base_addr;    
} alloc_block_resp;

alloc_block_resp alloc_block();

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
    physical_address buffer_base_addr;
} alloc_spec_frame_resp;

alloc_spec_frame_resp alloc_spec_frame(physical_address frame);

void clear_phys_mem_config();

unsigned int get_num_blocks_in_use();

unsigned int get_num_free_blocks();

typedef enum mem_mapped_io_status {
    MEM_MAPPED_IO_SUCCESS = 0,
    MEM_MAPPED_IO_FRAME_BASE_DNE = 1
} mem_mapped_io_status;

typedef struct mem_mapped_io_resp {
    mem_mapped_io_status status;
    bool has_mem_mapped_io;
} mem_mapped_io_resp;

mem_mapped_io_resp frame_has_mem_mapped_io(physical_address frame_base_addr);

bool phys_mem_needs_config();
