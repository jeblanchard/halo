#include "pte.h"
#include "stdio.h"

void add_pte_attrib(page_table_entry* entry, page_table_entry_attrib attrib) {
    *entry |= attrib;
}

void rm_pte_attrib(page_table_entry* entry, page_table_entry_attrib attrib) {
    *entry = *entry & ~(attrib);
}

void set_pte_frame_base_addr(page_table_entry* entry, physical_address phys_addr) {
    page_table_entry new_entry = 0;
    new_entry |= (phys_addr << 12);
    new_entry |= (*entry & 0xfff);

    *entry = new_entry;
}

physical_address get_pte_frame_base(page_table_entry* entry) {
    return (physical_address) *entry >> 12;
}

bool pte_is_writeable(page_table_entry* entry) {
    return (*entry & PTE_WRITABLE);
}

bool page_is_present(page_table_entry* entry) {
    return (*entry & PTE_PRESENT);
}

bool is_pte_attrib_set(page_table_entry* entry, page_table_entry_attrib attrib) {
    return (*entry & attrib);
}

page_table_entry new_pte() {
    return (page_table_entry) 0;
}

bool page_is_missing(page_table_entry* entry) {
    return !page_is_present(entry);
}

page_table new_page_table() {
    page_table new_pt = {};

    for (int i = 0; i < ENTRIES_PER_PAGE_TABLE; i++) {
        new_pt.entries[i] = new_pte();
    }

    return new_pt;
}
