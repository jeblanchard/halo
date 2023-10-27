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
    mem_map_entry * mem_map_entry_list_base_addr;
    unsigned int mem_map_num_entries;
} boot_info;
#pragma pack(pop)
