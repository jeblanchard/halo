#pragma once

#pragma pack(push, 1)
typedef struct memory_map_entry {
    unsigned int base_addr_low;

    // On a 32-bit system, we
    // will never use this.
    unsigned int base_addr_high;

    unsigned int length_low;

    // On a 32-bit system, we
    // will never use this.
    unsigned int length_high;

    unsigned int type;
} memory_map_entry;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct basic_memory_info {
    unsigned int type;
    unsigned int reserved;
} basic_memory_info;
#pragma pack(pop)



// A struct representing what the boot sector provides
// the kernel upon entry.
#pragma pack(push, 1)
typedef struct multiboot2_info {
    unsigned int total_size;
    unsigned int reserved;

    unsigned int mem_info_type;
    unsigned int mem_info_size;
    unsigned int num_kb_in_mem;

    unsigned int mem_map_info_type;
    unsigned int mem_map_info_size;
    unsigned int entry_size;
    unsigned int entry_version;
    memory_map_entry * mem_map_entry_list_base_addr;
    unsigned int mem_map_num_entries;

    unsigned int terminating_tag_type;
    unsigned int terminating_tag_size;
} multiboot2_info;
#pragma pack(pop)

typedef enum {
    AVAILABLE_RAM = 1,
    ACPI_INFO = 3,
    RESERVED_MEMORY = 4,
    DEFECTIVE_RAM_MODULES = 5
} memory_range_type;
