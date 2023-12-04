#include "pde.h"

void add_pde_attrib(page_dir_entry* entry, page_dir_entry_attrib attrib) {
    *entry |= attrib;
}

void rm_pde_attrib(page_dir_entry* entry, page_dir_entry_attrib attrib) {
    *entry = *entry & ~(attrib);
}

void set_pt_base_addr(page_dir_entry* entry, physical_address pt_base_addr) {
    *entry |= pt_base_addr << 12;
}

bool pde_is_present(page_dir_entry* entry) {
    return (*entry & PDE_PRESENT);
}

bool pde_is_user(page_dir_entry* entry) {
    return (*entry & PDE_USER);
}

bool pde_is_4mb(page_dir_entry* entry) {
    return (*entry & PDE_SIZE_4MB);
}

bool pde_is_writeable(page_dir_entry* entry) {
    return (*entry & PDE_WRITABLE);
}

bool is_pde_attrib_set(page_dir_entry* entry, page_dir_entry_attrib attrib) {
    return (*entry & attrib);
}

page_dir_entry new_pde() {
    return (page_dir_entry) 0;
}

bool is_kernel_pde(page_dir_entry* entry) {
    return !is_pde_attrib_set(entry, PDE_USER);
}
