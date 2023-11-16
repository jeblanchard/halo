#include "kernel/memory/physical_mem.h"
#include <stdbool.h>

typedef unsigned int page_dir_entry;

typedef enum page_dir_entry_flag {
    PRESENT = 1,
    WRITABLE = 2, 
    USER = 4,
    PWT = 8,
    PCD = 0x10,
    ACCESSED = 0x20,
    DIRTY = 0x40,
    SIZE_4MB = 0x80,
    CPU_GLOBAL = 0x100,
    LV4_GLOBAL = 0x200,
    FRAME = 0x7ffff000
} page_dir_entry_flag;

void add_pde_attrib(page_dir_entry* entry, page_dir_entry_flag attrib);

void rm_pde_attrib(page_dir_entry* entry, page_dir_entry_flag attrib);

void set_pde_frame(page_dir_entry* entry, physical_address phys_addr);

bool pde_is_present(page_dir_entry* entry);

bool pde_is_user(page_dir_entry* entry);

bool pde_is_4mb(page_dir_entry* entry);

bool pde_is_writeable(page_dir_entry* entry);

physical_address get_pde_frame(page_dir_entry* entry);

void enable_global(page_dir_entry* entry);
