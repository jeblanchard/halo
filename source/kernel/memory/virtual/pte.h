#pragma once

#include "kernel/memory/physical/physical_mem.h"
#include <stdbool.h>

typedef enum page_table_entry_attrib {
    PTE_PRESENT = 1,
    PTE_WRITABLE = 2,
    PTE_USER = 4,
    PTE_WRITE_THROUGH = 8,
    PTE_NOT_CACHEABLE = 0x10,
    PTE_ACCESSED = 0x20,
    PTE_DIRTY = 0x40,
    PTE_PAT = 0x80,
    PTE_CPU_GLOBAL = 0x100,
    PTE_LV4_GLOBAL = 0x200,
} page_table_entry_attrib;

typedef unsigned int page_table_entry;

void add_pte_attrib(page_table_entry* entry, page_table_entry_attrib attrib);

void rm_pte_attrib(page_table_entry* entry, page_table_entry_attrib attrib);

void set_pte_frame_base_addr(page_table_entry* entry, physical_address phys_addr);

physical_address get_pte_frame_base(page_table_entry* entry);

bool pte_is_writeable(page_table_entry* entry);

bool pte_is_present(page_table_entry* entry);

bool is_pte_attrib_set(page_table_entry* entry, page_table_entry_attrib attrib);

page_table_entry new_pte();
