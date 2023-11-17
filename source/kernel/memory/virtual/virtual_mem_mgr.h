#include "pte.h"
#include "pde.h"

typedef enum alloc_page_resp {
    SUCCESS = 0,
    NO_MEM_AVAIL = 1,
    GENERAL_FAILURE = 2
} alloc_page_resp;

alloc_page_resp alloc_page(page_table_entry* entry);

void free_page(page_table_entry* entry);

typedef unsigned int virtual_address;

unsigned int get_page_table_index(virtual_address addr);

page_table_entry* get_page_table_entry(page_table_entry* page_table,
                                       virtual_address addr);

unsigned int get_page_dir_index(virtual_address addr);

page_dir_entry* get_page_dir_entry(page_dir_entry* page_dir_table, virtual_address addr);
