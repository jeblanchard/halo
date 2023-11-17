#include "virtual_mem_mgr.h"
#include "kernel/memory/virtual/pte.h"
#include "kernel/memory/physical/physical_mem.h"

alloc_page_resp alloc_page(page_table_entry* entry) {
    block_alloc_resp resp = alloc_block();
    if (resp.status == BLOCK_ALLOC_SUCCESS) {
    } else if (resp.status == NO_FREE_BLOCKS) {
        return NO_MEM_AVAIL;
    } else {
        return GENERAL_FAILURE;
    }

    set_pte_frame(entry, (physical_address) resp.buffer);
    add_pte_attrib(entry, PTE_PRESENT);

    return SUCCESS;
}

void free_page(page_table_entry* entry) {
    (void) entry;

    physical_address addr_to_free = get_pte_frame(entry);
    free_block(addr_to_free);

    rm_pte_attrib(entry, PTE_PRESENT);
}

unsigned int get_page_table_index(virtual_address addr) {
    return (addr >> 12) & 0x3ff;
}

page_table_entry* get_page_table_entry(page_table_entry* page_table,
                                       virtual_address addr) {

    unsigned int pt_index = get_page_table_index(addr);
    return &page_table[pt_index];
}

unsigned int get_page_dir_index(virtual_address addr) {
    return (addr >> 22) & 0x3ff;
}

page_dir_entry* get_page_dir_entry(page_dir_entry* page_dir_table, virtual_address addr) {
    unsigned int pd_index = get_page_dir_index(addr);
    return &page_dir_table[pd_index];
}
