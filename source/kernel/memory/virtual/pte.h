#include "kernel/memory/physical_mem.h"
#include <stdbool.h>

typedef enum page_table_entry_flag {
    PRESENT = 1,
    WRITABLE = 2,
    USER = 4,
    WRITE_THROUGH = 8,
    NOT_CACHEABLE = 0x10,
    ACCESSED = 0x20,
    DIRTY = 0x40,
    PAT = 0x80,
    CPU_GLOBAL = 0x100,
    LV4_GLOBAL = 0x200,
    PTE_FRAME = 0x7FFFF000
} page_table_entry_flag;

typedef unsigned int page_table_entry;

void add_pte_attrib(page_table_entry* entry, page_table_entry_flag attrib);

void rm_pte_attrib(page_table_entry* entry, page_table_entry_flag attrib);

void set_pte_frame(page_table_entry* entry, physical_address phys_addr);

physical_address get_pte_frame(page_table_entry* entry);

bool pte_is_writeable(page_table_entry* entry);

bool pte_is_present(page_table_entry* entry);
