#include "pde.h"

void add_pde_attrib(page_dir_entry* entry, page_dir_entry_attrib attrib) {
    *entry |= attrib;
}

void rm_pde_attrib(page_dir_entry* entry, page_dir_entry_attrib attrib) {
    *entry = *entry & ~(attrib);
}

void set_pt_base_addr(page_dir_entry* entry, physical_address pt_base_addr) {
    physical_address squished_addr = pt_base_addr >> 12;
    *entry |= squished_addr << 12;
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

bool is_valid_pt_squished_base_addr(physical_address squished_addr) {
    if (squished_addr == 0) {
        return false;
    }

    return true;
}

pt_base_addr_resp get_page_table_base_addr(page_dir_entry* entry) {
    physical_address squished_addr = *entry >> 12;
    if (is_valid_pt_squished_base_addr(squished_addr)) {
        return (pt_base_addr_resp) {
            status: PT_BASE_ADDR_SUCCESS,
            pt_base_addr: squished_addr << 12
        };
    }

    return (pt_base_addr_resp) {
        status: PT_DNE,
        pt_base_addr: 0
    };
}

page_dir new_page_dir() {
    page_dir new_pd = {};
    for (int i = 0; i < ENTRIES_PER_PAGE_DIR; i++) {
        new_pd.entries[i] = new_pde();
    }

    return new_pd;
}
