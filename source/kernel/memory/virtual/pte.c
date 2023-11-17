#include "pte.h"

void add_pte_attrib(page_table_entry* entry, page_table_entry_flag attrib) {
    *entry |= attrib;
}

void rm_pte_attrib(page_table_entry* entry, page_table_entry_flag attrib) {
    *entry = *entry & ~(attrib);
}

void set_pte_frame(page_table_entry* entry, physical_address phys_addr) {
    page_table_entry new_entry = 0;
    new_entry |= (phys_addr << 12);
    new_entry |= (*entry & 0xfff);

    *entry = new_entry;
}

physical_address get_pte_frame(page_table_entry* entry) {
    return (physical_address) *entry >> 12;
}

bool pte_is_writeable(page_table_entry* entry) {
    return (*entry & PTE_WRITABLE);
}

bool pte_is_present(page_table_entry* entry) {
    return (*entry & PTE_PRESENT);
}
