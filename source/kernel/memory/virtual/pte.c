#include "pte.h"

void add_pte_attrib(page_table_entry* entry, page_table_entry_flag attrib) {
    *entry |= attrib;
}

void rm_pte_attrib(page_table_entry* entry, page_table_entry_flag attrib) {
    *entry = *entry & ~(attrib);
}

void set_pte_frame(page_table_entry* entry, physical_address phys_addr) {
    *entry |= phys_addr << 12;
}

physical_address get_pte_frame(page_table_entry* entry) {
    return *entry >> 12;
}

bool pte_is_writeable(page_table_entry* entry) {
    return (*entry & WRITABLE);
}

bool pte_is_present(page_table_entry* entry) {
    return (*entry & PRESENT);
}
