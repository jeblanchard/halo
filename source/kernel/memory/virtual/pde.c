#include "pde.h"

void add_pde_attrib(page_dir_entry* entry, page_dir_entry_flag attrib) {
    *entry |= attrib;
}

void rm_pde_attrib(page_dir_entry* entry, page_dir_entry_flag attrib) {
    *entry = *entry & ~(attrib);
}

void set_pde_frame(page_dir_entry* entry, physical_address phys_addr) {
    *entry |= phys_addr << 12;
}

bool pde_is_present(page_dir_entry* entry) {
    return (*entry & PRESENT);
}

bool pde_is_user(page_dir_entry* entry) {
    return (*entry & USER);
}

bool pde_is_4mb(page_dir_entry* entry) {
    return (*entry & SIZE_4MB);
}

bool pde_is_writeable(page_dir_entry* entry) {
    return (*entry & WRITABLE);
}

physical_address get_pde_frame(page_dir_entry* entry) {
    return *entry >> 12;
}

void enable_global(page_dir_entry* entry) {
    *entry |= CPU_GLOBAL;
}
