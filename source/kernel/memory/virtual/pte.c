#include "pte.h"
#include "stdio.h"

void set_pte_attrib(page_table_entry* entry, page_table_entry_attrib attrib) {
    *entry |= attrib;
}

void rm_pte_attrib(page_table_entry* entry, page_table_entry_attrib attrib) {
    *entry = *entry & ~(attrib);
}

bool frame_base_addr_is_misaligned(physical_address frame_base_addr) {
    if (frame_base_addr % PAGE_SIZE_IN_BYTES == 0) {
        return false;
    }

    return true;
}

set_pte_frame_base_addr_status set_pte_frame_base_addr(page_table_entry* entry, physical_address frame_base_addr) {
    if (frame_base_addr_is_misaligned(frame_base_addr)) {
        return FRAME_BASE_ADDR_NOT_ALIGNED;
    }

    page_table_entry new_entry = new_pte();

    physical_address squished_frame_base_addr = frame_base_addr >> 12;
    new_entry |= (squished_frame_base_addr << 12);
    new_entry |= (*entry & 0xfff);

    *entry = new_entry;

    return SET_PTE_FRAME_BASE_ADDR_SUCCESS;
}

physical_address get_pte_frame_base_addr(page_table_entry* entry) {
    physical_address squished_addr = *entry >> 12;
    return squished_addr << 12;
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
