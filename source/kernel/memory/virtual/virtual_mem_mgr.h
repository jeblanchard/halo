#include "pte.h"
#include "pde/pde.h"

typedef enum alloc_page_status {
    ALLOC_PAGE_SUCCESS = 0,
    ALLOC_PAGE_NO_MEM_AVAIL = 1,
    ALLOC_PAGE_GEN_FAILURE = 2
} alloc_page_status;

alloc_page_status alloc_page(page_table_entry* entry);

void free_page(page_table_entry* entry);

typedef unsigned int virtual_address;

unsigned int get_page_table_index(virtual_address addr);

#define ENTRIES_PER_PAGE_TABLE 1024

#pragma pack(push, 1)
typedef struct page_table {
    page_table_entry entries[ENTRIES_PER_PAGE_TABLE];
} page_table;
#pragma pack(pop)

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

map_page_status map_page_base_addr(physical_address phys_addr, virtual_address virt_addr);

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

void clear_vm_config();

page_table new_page_table();

#define MAX_32_BIT_VIRT_MEM_ADDR 0xffffffff

#define IO_BASE_ADDR 0

#define ONE_MB 0x10000
#define IO_MAX_ADDR ONE_MB - 1

#define THREE_GB 0xc0000000
#define USER_SPACE_BASE_ADDR IO_MAX_ADDR + 1
#define USER_SPACE_MAX_ADDR THREE_GB - 1

#define KERNEL_SPACE_BASE_VIRT_ADDR USER_SPACE_BASE_ADDR + 1
#define KERNEL_SPACE_MAX_VIRT_ADDR MAX_32_BIT_VIRT_MEM_ADDR

typedef enum init_vm_status {
    INIT_VM_SUCCESS = 0,
    COULD_NOT_INIT_IO_MEM = 3,
    COULD_NOT_INIT_KERNEL_MEM = 4,
    INIT_VM_GENERAL_FAILURE = 2,
    INIT_VM_FAILED_ALLOC_PT = 5,
    INIT_VM_FAILED_ALLOC_ALL_PTES = 6,
    INIT_VM_FAILED_LOADING_PD = 7
} init_vm_status;

init_vm_status init_virtual_mem();

physical_address get_page_table_base_addr(page_dir_entry* entry);

unsigned int get_pages_in_use();
