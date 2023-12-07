#pragma once

#include "kernel/memory/physical/physical_mem.h"
#include <stdbool.h>

typedef unsigned int page_dir_entry;

typedef enum page_dir_entry_attrib {
    PDE_PRESENT = 1,
    PDE_WRITABLE = 2, 
    PDE_USER = 4,
    PDE_PAGE_LVL_WRITE_THROUGH = 8,
    PDE_PAGE_LVL_CACHE_DISABLE = 0x10,
    PDE_ACCESSED = 0x20,
    PDE_SIZE_4MB = 0x80,
} page_dir_entry_attrib;

void add_pde_attrib(page_dir_entry* entry, page_dir_entry_attrib attrib);

void rm_pde_attrib(page_dir_entry* entry, page_dir_entry_attrib attrib);

void set_pt_base_addr(page_dir_entry* entry, physical_address pt_base_addr);

bool pde_is_present(page_dir_entry* entry);

bool pde_is_user(page_dir_entry* entry);

bool pde_is_4mb(page_dir_entry* entry);

bool pde_is_writeable(page_dir_entry* entry);

void enable_global(page_dir_entry* entry);

bool is_pde_attrib_set(page_dir_entry* entry, page_dir_entry_attrib attrib);

page_dir_entry new_pde();

bool is_kernel_pde(page_dir_entry* entry);

#define ENTRIES_PER_PAGE_DIR 1024

typedef struct page_dir {
    page_dir_entry entries[ENTRIES_PER_PAGE_DIR];
} page_dir __attribute__ ((aligned (4096)));

typedef enum pt_base_addr_status {
    PT_DNE = 0,
    PT_BASE_ADDR_SUCCESS = 1
} pt_base_addr_status;

typedef struct pt_base_addr_resp {
    pt_base_addr_status status;
    physical_address pt_base_addr;
} pt_base_addr_resp;

pt_base_addr_resp get_page_table_base_addr(page_dir_entry* entry);

page_dir new_page_dir();
