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

#define ENTRIES_PER_PAGE_TABLE 1024

typedef struct page_table {
    page_table_entry entries[ENTRIES_PER_PAGE_TABLE];
} page_table;

page_table_entry* get_page_table_entry(page_table* page_table,
                                       virtual_address addr);

unsigned int get_page_dir_index(virtual_address addr);

#define ENTRIES_PER_PAGE_DIR 1024

typedef struct page_dir {
    page_dir_entry entries[ENTRIES_PER_PAGE_DIR];
} page_dir;

page_dir_entry* get_page_dir_entry(page_dir* page_dir, virtual_address addr);

typedef enum load_new_pd_status {
    PD_ALREADY_LOADED = 0,
    LOAD_NEW_PD_SUCCESS = 1
} load_new_pd_status;

load_new_pd_status load_new_pd(physical_address new_pdbr_base_addr);

typedef enum get_curr_pdbr_status {
    PDBR_NEEDS_LOADING = 0,
    GET_CURR_PDBR_SUCCESS = 1
} get_curr_pdbr_status;

typedef struct get_curr_pdbr_resp {
    get_curr_pdbr_status status;
    physical_address curr_pdbr;
} get_curr_pdbr_resp;

get_curr_pdbr_resp get_curr_pdbr();

void flush_tlb_entry(virtual_address addr_to_flush);

typedef enum map_page_status {
    MP_NOT_ENOUGH_MEM = 0,
    MP_GENERAL_FAILURE = 1,
    MP_SUCCESS = 2
} map_page_status;

map_page_status map_page(physical_address phys_addr, virtual_address virt_addr);

page_dir new_page_dir();

typedef enum get_phys_addr_status {
    NO_INITIALIZED_PD = 0,
    GET_PHYS_ADDR_SUCCESS = 1,
    GET_PHYS_ADDR_GEN_FAILURE = 2
} get_phys_addr_status;

typedef struct get_phys_addr_resp {
    get_phys_addr_status status;
    physical_address phys_addr;
} get_phys_addr_resp;

get_phys_addr_resp get_phys_addr(virtual_address virt_addr);

typedef enum get_curr_pd_status {
    PD_INIT_NEEDED = 0,
    GET_CURR_PD_SUCCESS = 1
} get_curr_pd_status;

typedef struct get_curr_pd_resp {
    get_curr_pd_status status;
    page_dir* curr_pd;
} get_curr_pd_resp;

get_curr_pd_resp get_curr_pd();

#define BYTES_PER_PAGE 4096

unsigned int get_frame_offset(virtual_address virt_addr);

typedef enum new_virt_addr_status {
    NEW_VIRT_ADDR_SUCCESS = 0,
    NEW_VIRT_ADDR_BAD_PD_INDEX = 1,
    NEW_VIRT_ADDR_BAD_PT_INDEX = 2,
    NEW_VIRT_ADDR_BAD_FRAME_OFFSET = 3
} new_virt_addr_status;

typedef struct new_virt_addr_resp {
    new_virt_addr_status status;
    virtual_address virt_addr;
} new_virt_addr_resp;

new_virt_addr_resp new_virt_addr(unsigned int pd_index,
                                 unsigned int pt_index,
                                 unsigned int page_offset);

void clear_vm_init();

page_table new_page_table();
