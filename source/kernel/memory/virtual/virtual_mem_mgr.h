#include "pte.h"
#include "pde.h"

typedef enum alloc_page_status {
    ALLOC_PAGE_SUCCESS = 0,
    ALLOC_PAGE_FAILED_SETTING_PTE_FRAME = 1,
    ALLOC_PAGE_PHYS_MEM_IS_FULL = 2,
    ALLOC_PAGE_FAILED_ALLOC_BLOCK = 3
} alloc_page_status;

alloc_page_status alloc_page(page_table_entry* entry);

void free_page(page_table_entry* entry);

typedef unsigned int virtual_address;

unsigned int get_page_table_index(virtual_address addr);

page_table_entry* get_page_table_entry(page_table* page_table,
                                       virtual_address addr);

unsigned int get_page_dir_index(virtual_address addr);

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
    MAP_PAGE_NOT_ENOUGH_MEM = 0,
    MAP_PAGE_SUCCESS = 1,
    MAP_PAGE_FAILED_GETTING_PT_BASE_ADDR = 2,
    MAP_PAGE_PDE_OF_VA_MISSING = 3
} map_page_status;

map_page_status map_page_base_addr(physical_address new_page_base_addr,
                                   virtual_address virt_addr_to_map);

typedef enum get_phys_addr_status {
    NO_INITIALIZED_PD = 0,
    GET_PHYS_ADDR_SUCCESS = 1,
    GET_PHYS_ADDR_FAILED_GETTING_CURR_PD = 2,
    GET_PHYS_ADDR_PT_DNE = 3,
    GET_PHYS_ADDR_FAILED_GET_PT_ADDR = 4
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

#define MAX_32_BIT_VIRT_MEM_ADDR 0xffffffff
#define MIN_32_BIT_VIRT_MEM_ADDR 0

#define THREE_GB 0xc0000000
#define USER_SPACE_BASE_VIRT_ADDR 0
#define USER_SPACE_MAX_VIRT_ADDR (THREE_GB - 1)

#define KERNEL_SPACE_BASE_VIRT_ADDR (USER_SPACE_MAX_VIRT_ADDR + 1)
#define KERNEL_SPACE_MAX_VIRT_ADDR MAX_32_BIT_VIRT_MEM_ADDR

typedef enum init_vm_status {
    INIT_VM_FAILED_MAPPING_ALL_VIRT_ADDR = 0,
    INIT_VM_FAILED_LOADING_PD = 1,
    INIT_VM_SUCCESS = 0
} init_vm_status;

init_vm_status init_virtual_mem();

unsigned int get_num_pages_in_use();

typedef enum free_pd_status {
    FREE_PD_SUCCESS = 0,
    FREE_PD_FAILED_GETTING_PT = 1
} free_pd_status;

free_pd_status free_pd(page_dir* pd);

typedef enum kernel_phys_addr_status {
    TRANS_TO_KERNEL_PA_SUCCESS = 0,
    NOT_A_KERNEL_VIRT_ADDR = 1
} kernel_phys_addr_status;

typedef struct kernel_phys_addr_trans_resp {
    kernel_phys_addr_status status;
    physical_address kernel_phys_addr;
} kernel_phys_addr_trans_resp;

kernel_phys_addr_trans_resp trans_to_kernel_phys_addr(virtual_address virt_addr);
