#pragma once

#include "kernel/memory/physical/physical_mem.h"
#include <stdbool.h>

typedef unsigned int page_dir_entry;

typedef enum page_dir_entry_attrib {
    PDE_PRESENT = 1,
    PDE_WRITABLE = 2, 
    PDE_USER = 4,
    PDE_PWT = 8,
    PDE_PCD = 0x10,
    PDE_ACCESSED = 0x20,
    PDE_DIRTY = 0x40,
    PDE_SIZE_4MB = 0x80,
    PDE_CPU_GLOBAL = 0x100,
    PDE_LV4_GLOBAL = 0x200,
} page_dir_entry_attrib;

void add_pde_attrib(page_dir_entry* entry, page_dir_entry_attrib attrib);

void rm_pde_attrib(page_dir_entry* entry, page_dir_entry_attrib attrib);

void set_pt_base_addr(page_dir_entry* entry, physical_address phys_addr);

bool pde_is_present(page_dir_entry* entry);

bool pde_is_user(page_dir_entry* entry);

bool pde_is_4mb(page_dir_entry* entry);

bool pde_is_writeable(page_dir_entry* entry);

physical_address get_page_table_base_addr(page_dir_entry* entry);

void enable_global(page_dir_entry* entry);

bool is_pde_attrib_set(page_dir_entry* entry, page_dir_entry_attrib attrib);

page_dir_entry new_pde();
