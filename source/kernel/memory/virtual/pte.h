#pragma once

#include "kernel/memory/physical/physical_mem.h"
#include <stdbool.h>

typedef enum page_table_entry_attrib {
    PTE_PRESENT = 1,
    PTE_WRITABLE = 2,
    PTE_USER = 4,
    PTE_WRITE_THROUGH = 8,
    PTE_PAGE_LVL_CACHE_DISABLE = 0x10,
    PTE_ACCESSED = 0x20,
    PTE_DIRTY = 0x40,
    PTE_PAT = 0x80,
    PTE_CPU_GLOBAL = 0x100,
} page_table_entry_attrib;

typedef unsigned int page_table_entry;

void set_pte_attrib(page_table_entry* entry, page_table_entry_attrib attrib);

void rm_pte_attrib(page_table_entry* entry, page_table_entry_attrib attrib);

void set_pte_frame_base_addr(page_table_entry* entry, physical_address phys_addr);

physical_address get_pte_frame_base(page_table_entry* entry);

bool pte_is_writeable(page_table_entry* entry);

bool page_is_present(page_table_entry* entry);

bool is_pte_attrib_set(page_table_entry* entry, page_table_entry_attrib attrib);

page_table_entry new_pte();

bool page_is_missing(page_table_entry* entry);

#define ENTRIES_PER_PAGE_TABLE 1024

#pragma pack(push, 1)
typedef struct page_table {
    page_table_entry entries[ENTRIES_PER_PAGE_TABLE];
} page_table __attribute__ ((aligned (4096)));
#pragma pack(pop)

page_table new_page_table();
