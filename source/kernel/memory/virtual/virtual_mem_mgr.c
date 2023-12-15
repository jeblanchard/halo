#include "virtual_mem_mgr.h"
#include "kernel/memory/virtual/pte.h"
#include "kernel/memory/physical/physical_mem.h"
#include "stddef.h"
#include "kernel/memory/virtual/pde/pde.h"
#include "stdio.h"

static unsigned int pages_in_use;

unsigned int get_num_pages_in_use() {
    return pages_in_use;
}

alloc_page_status alloc_page(page_table_entry* entry) {
    alloc_block_resp phys_block_resp = alloc_block();
    if (phys_block_resp.status == ALLOC_BLOCK_SUCCESS) {
    } else if (phys_block_resp.status == ALLOC_BLOCK_ALL_BLOCKS_IN_USE) {
        return ALLOC_PAGE_PHYS_MEM_IS_FULL;
    } else {
        return ALLOC_PAGE_FAILED_ALLOC_BLOCK;
    }

    set_pte_frame_base_addr_status block_as_frame_status = \
        set_pte_frame_base_addr(entry, phys_block_resp.buffer_base_addr);

    if (block_as_frame_status == SET_PTE_FRAME_BASE_ADDR_SUCCESS) {
    } else {
        return ALLOC_PAGE_FAILED_SETTING_PTE_FRAME;
    }

    set_pte_attrib(entry, PTE_PRESENT);
    pages_in_use += 1;

    return ALLOC_PAGE_SUCCESS;
}

void free_page(page_table_entry* pte_to_free) {
    physical_address addr_to_free = get_pte_frame_base_addr(pte_to_free);
    free_block(addr_to_free);

    if (page_is_present(pte_to_free)) {
        pages_in_use -= 1;
    }

    rm_pte_attrib(pte_to_free, PTE_PRESENT);
}

unsigned int get_page_table_index(virtual_address addr) {
    return (addr >> 12) & 0x3ff;
}

page_table_entry* get_page_table_entry(page_table* pt,
                                       virtual_address addr) {

    unsigned int pt_index = get_page_table_index(addr);
    return &pt -> entries[pt_index];
}

unsigned int get_page_dir_index(virtual_address addr) {
    return (addr >> 22) & 0x3ff;
}

page_dir_entry* get_page_dir_entry(page_dir* pd, virtual_address addr) {
    unsigned int pd_index = get_page_dir_index(addr);
    return &pd -> entries[pd_index];
}

static physical_address current_pdbr;

static bool pd_has_been_loaded = false;

get_curr_pdbr_resp get_curr_pdbr() {
    if (pd_has_been_loaded) {
        return (get_curr_pdbr_resp) {
            status: GET_CURR_PD_SUCCESS,
            curr_pdbr: current_pdbr};
    }

    return (get_curr_pdbr_resp) {
        status: PDBR_NEEDS_LOADING,
        curr_pdbr: 0
    };
}

extern void load_pdbr_asm(physical_address new_pd_base_addr);

static page_dir* current_page_dir;

get_curr_pd_resp get_curr_pd() {
    if (pd_has_been_loaded) {
        return (get_curr_pd_resp) {
            status: GET_CURR_PD_SUCCESS,
            curr_pd: current_page_dir
        };
    }

    return (get_curr_pd_resp) {
        status: PD_INIT_NEEDED,
        curr_pd: NULL
    };
}

load_new_pd_status load_new_pd(physical_address new_pd_base_addr) {
    if (pd_has_been_loaded) {
        return PD_ALREADY_LOADED;
    } else {
        current_pdbr = new_pd_base_addr;
    }

    current_page_dir = (page_dir*) new_pd_base_addr;

    load_pdbr_asm(current_pdbr);

    pd_has_been_loaded = true;

    return LOAD_NEW_PD_SUCCESS;
}

bool pde_is_missing(page_dir_entry* entry) {
    return !(pde_is_present(entry));
}

map_page_status map_page_base_addr(physical_address new_page_base_addr,
                                   virtual_address virt_addr_to_map) {

    unsigned int pd_index = get_page_dir_index(virt_addr_to_map);
    page_dir_entry* pde_of_va = &current_page_dir -> entries[pd_index];
    if (pde_is_missing(pde_of_va)) {
        return MAP_PAGE_PDE_OF_VA_MISSING;
    }   

    pt_base_addr_resp pt_resp = get_page_table_base_addr(pde_of_va);
    if (pt_resp.status == PT_BASE_ADDR_SUCCESS) {
    } else {
        return MAP_PAGE_FAILED_GETTING_PT_BASE_ADDR;
    }

    page_table* pt_of_va = (page_table*) pt_resp.pt_base_addr;
    page_table_entry* pte_of_va = get_page_table_entry(pt_of_va, virt_addr_to_map);

    set_pte_frame_base_addr(pte_of_va, new_page_base_addr);
    set_pte_attrib(pte_of_va, PTE_PRESENT);

    return MAP_PAGE_SUCCESS;
}

get_phys_addr_resp get_phys_addr(virtual_address virt_addr) {
    get_curr_pd_resp resp = get_curr_pd();
    if (resp.status == GET_CURR_PD_SUCCESS) {
    } else {
        return (get_phys_addr_resp) {
            status: GET_PHYS_ADDR_FAILED_GETTING_CURR_PD,
            phys_addr: 0};
    }

    unsigned int pd_index = get_page_dir_index(virt_addr);
    page_dir_entry* pde = &resp.curr_pd -> entries[pd_index];

    pt_base_addr_resp pt_resp = get_page_table_base_addr(pde);
    if (pt_resp.status == PT_BASE_ADDR_SUCCESS) {
    } else if (pt_resp.status == PT_DNE) {
        return (get_phys_addr_resp) {
            status: GET_PHYS_ADDR_PT_DNE,
            phys_addr: 0};
    } else {
        return (get_phys_addr_resp) {
            status: GET_PHYS_ADDR_FAILED_GET_PT_ADDR,
            phys_addr: 0};
    }

    page_table* pt = (page_table*) pt_resp.pt_base_addr;

    unsigned int pt_index = get_page_table_index(virt_addr);
    page_table_entry* pte = &pt -> entries[pt_index];

    physical_address frame_base_addr = get_pte_frame_base_addr(pte);
    unsigned int page_offset = get_frame_offset(virt_addr);
    physical_address translation = frame_base_addr + page_offset;

    return (get_phys_addr_resp) {status: GET_PHYS_ADDR_SUCCESS, phys_addr: translation};
}

unsigned int get_frame_offset(virtual_address virt_addr) {
    return (unsigned int) (virt_addr << 20) >> 20;
}

void set_va_pd_index(virtual_address* virt_addr, unsigned int pd_index) {
    unsigned int frame_offset = get_frame_offset(*virt_addr);
    unsigned int pt_index = get_page_table_index(*virt_addr);

    virtual_address new_virt_addr = 0;
    new_virt_addr |= frame_offset;
    new_virt_addr |= pt_index << 12;
    new_virt_addr |= pd_index << 22;

    *virt_addr = new_virt_addr;
}

void set_va_pt_index(virtual_address* virt_addr, unsigned int pt_index) {
    unsigned int frame_offset = get_frame_offset(*virt_addr);
    unsigned int pd_index = get_page_dir_index(*virt_addr);

    virtual_address new_virt_addr = 0;
    new_virt_addr |= frame_offset;
    new_virt_addr |= pd_index << 22;
    new_virt_addr |= pt_index << 12;

    *virt_addr = new_virt_addr;
}

void set_va_frame_offset(virtual_address* virt_addr, unsigned int frame_offset) {
    unsigned int pt_index = get_page_table_index(*virt_addr);
    unsigned int pd_index = get_page_dir_index(*virt_addr);

    virtual_address new_virt_addr = 0;
    new_virt_addr |= frame_offset;
    new_virt_addr |= pd_index << 22;
    new_virt_addr |= pt_index << 12;

    *virt_addr = new_virt_addr;
}

new_virt_addr_resp new_virt_addr(unsigned int pd_index,
                                 unsigned int pt_index,
                                 unsigned int page_offset) {

    if (pd_index >= ENTRIES_PER_PAGE_DIR) {
        return (new_virt_addr_resp) {status: NEW_VIRT_ADDR_BAD_PD_INDEX};
    }

    if (pt_index >= ENTRIES_PER_PAGE_TABLE) {
        return (new_virt_addr_resp) {status: NEW_VIRT_ADDR_BAD_PT_INDEX};
    }

    if (page_offset >= BYTES_PER_PAGE) {
        return (new_virt_addr_resp) {status: NEW_VIRT_ADDR_BAD_FRAME_OFFSET};
    }

    virtual_address new_virt_addr = 0;
    set_va_pd_index(&new_virt_addr, pd_index);
    set_va_pt_index(&new_virt_addr, pt_index);
    set_va_frame_offset(&new_virt_addr, page_offset);

    return (new_virt_addr_resp) {status: NEW_VIRT_ADDR_SUCCESS,
                                 virt_addr: new_virt_addr};
}

void clear_vm_config() {
    pd_has_been_loaded = false;
}

static page_dir default_pd;
static page_table all_page_tables[ENTRIES_PER_PAGE_DIR];

void init_all_page_tables() {
    for (int i = 0; i < ENTRIES_PER_PAGE_DIR; i++) {
        all_page_tables[i] = new_page_table();
    }
}

void set_all_pte_to_present(page_table* pt) {
    for (int i = 0; i < ENTRIES_PER_PAGE_TABLE; i++) {
        set_pte_attrib(&pt -> entries[i], PTE_PRESENT);
    }
}

void assign_all_pt_addr() {
    default_pd = new_page_dir();
    for (int i = 0; i < ENTRIES_PER_PAGE_DIR; i++) {
        page_dir_entry* pde_being_init = &default_pd.entries[i];
        page_table* pt_being_init = &all_page_tables[i];
        set_pt_base_addr(
            pde_being_init,
            (physical_address) pt_being_init);

        set_pde_attrib(pde_being_init, PDE_PRESENT);
        set_all_pte_to_present(pt_being_init);
    }
}

void free_pt(page_table* pt) {
    for (int i = 0; i < ENTRIES_PER_PAGE_TABLE; i++) {
        free_page(&pt -> entries[i]);
    }
}

bool is_a_pt_base_addr(virtual_address addr) {
    if (addr % (BYTES_PER_PAGE * ENTRIES_PER_PAGE_TABLE) == 0) {
        return true;
    }

    return false;
}

physical_address trans_to_io_phys_addr(virtual_address virt_addr) {
    return (physical_address) virt_addr;
}

typedef enum page_has_mem_map_io_status {
    PAGE_HAS_MEM_MAP_IO_SUCCESS = 0,
    PAGE_HAS_MEM_MAP_IO_FAILED_GETTING_FRAME_IO_POTENTIAL = 1
} page_has_mem_map_io_status;

typedef struct page_has_mem_map_io_resp {
    page_has_mem_map_io_status status;
    bool has_mem_map_io;
} page_has_mem_map_io_resp;

page_has_mem_map_io_resp page_has_mem_map_io(virtual_address page_base_virt_addr) {
    physical_address frame_base_phys_addr = \
        trans_to_io_phys_addr(page_base_virt_addr);

    mem_mapped_io_resp actual_potential_resp = \
        frame_has_mem_mapped_io(frame_base_phys_addr);

    if (actual_potential_resp.status == MEM_MAPPED_IO_SUCCESS) {
    } else {
        return (page_has_mem_map_io_resp) {
            status: PAGE_HAS_MEM_MAP_IO_FAILED_GETTING_FRAME_IO_POTENTIAL,
            has_mem_map_io: false
        };
    }

    return (page_has_mem_map_io_resp) {
        status: PAGE_HAS_MEM_MAP_IO_SUCCESS,
        has_mem_map_io: actual_potential_resp.has_mem_mapped_io
    };
}

bool is_user_virt_addr(virtual_address addr) {
    if (addr == USER_SPACE_BASE_VIRT_ADDR ||
        (addr > USER_SPACE_BASE_VIRT_ADDR &&
         addr <= USER_SPACE_MAX_VIRT_ADDR)) {
            return true;
    }

    return false;
}

bool is_kernel_virt_addr(virtual_address addr) {
    if (addr >= KERNEL_SPACE_BASE_VIRT_ADDR &&
        addr <= KERNEL_SPACE_MAX_VIRT_ADDR) {
            return true;
    }

    return false;
}

free_pd_status free_pd(page_dir* pd) {
    for (int i = 0; i < ENTRIES_PER_PAGE_DIR; i++) {
        pt_base_addr_resp entry_pt_resp = get_page_table_base_addr(&pd -> entries[i]);
        if (entry_pt_resp.status == PT_BASE_ADDR_SUCCESS) {
        } else {
            return FREE_PD_FAILED_GETTING_PT;
        }

        page_table* entry_pt = (page_table*) entry_pt_resp.pt_base_addr;
        free_pt(entry_pt);
    }

    return FREE_PD_SUCCESS;
}

kernel_phys_addr_trans_resp trans_to_kernel_phys_addr(virtual_address virt_addr) {
    if (is_kernel_virt_addr(virt_addr)) {
        extern physical_address KERNEL_PHYS_ADDRESS;
        physical_address kernel_phys_addr = \
            virt_addr - (KERNEL_SPACE_BASE_VIRT_ADDR - KERNEL_PHYS_ADDRESS);

        return (kernel_phys_addr_trans_resp) {
            status: TRANS_TO_KERNEL_PA_SUCCESS,
            kernel_phys_addr: kernel_phys_addr
        };
    }

    return (kernel_phys_addr_trans_resp) {
        status: NOT_A_KERNEL_VIRT_ADDR,
        kernel_phys_addr: 0
    };
}

typedef enum alloc_io_page_status {
    ALLOC_IO_PAGE_SUCCESS = 0,
    ALLOC_IO_PAGE_IO_FRAME_IS_LABELED_AS_FREE = 1
} alloc_io_page_status;

alloc_io_page_status alloc_io_page(page_table_entry* io_pte,
                                   virtual_address io_page_base_addr) {
    
    physical_address io_pa_trans = trans_to_io_phys_addr(io_page_base_addr);

    alloc_spec_frame_resp io_virt_addr_phys_alloc_resp = alloc_spec_frame(io_pa_trans);
    if (io_virt_addr_phys_alloc_resp.status == ALLOC_SPEC_FRAME_IN_USE) {
        set_pte_attrib(io_pte, PTE_PRESENT);
        set_pte_frame_base_addr(io_pte, io_pa_trans);
    } else {
        return ALLOC_IO_PAGE_IO_FRAME_IS_LABELED_AS_FREE;
    }

    return ALLOC_IO_PAGE_SUCCESS;
}

typedef enum alloc_kernel_page_status {
    ALLOC_KERNEL_PAGE_SUCCESS = 0,
    ALLOC_KERNEL_PAGE_FAILED_TRANS_TO_PHYS_ADDR = 1
} alloc_kernel_page_status;

alloc_kernel_page_status alloc_kernel_page(page_table_entry* kernel_pte,
                                           virtual_address kernel_page_base_addr) {
    
    kernel_phys_addr_trans_resp kernel_pa_trans_resp = trans_to_kernel_phys_addr(kernel_page_base_addr);
    if (kernel_pa_trans_resp.status == TRANS_TO_KERNEL_PA_SUCCESS) {
    } else {
        return ALLOC_KERNEL_PAGE_FAILED_TRANS_TO_PHYS_ADDR;
    }

    alloc_spec_frame_resp kernel_phys_addr_alloc_resp = \
        alloc_spec_frame(kernel_pa_trans_resp.kernel_phys_addr);

    if (kernel_phys_addr_alloc_resp.status == ALLOC_SPEC_FRAME_SUCCESS) {
        set_pte_attrib(kernel_pte, PTE_PRESENT);
        set_pte_frame_base_addr(kernel_pte, kernel_phys_addr_alloc_resp.buffer_base_addr);
    }

    return ALLOC_KERNEL_PAGE_SUCCESS;
}

typedef enum alloc_user_page_status {
    ALLOC_USER_PAGE_SUCCESS = 0,
    ALLOC_USER_PAGE_FAILED_ALLOC_BLOCK = 1
} alloc_user_page_status;

alloc_user_page_status alloc_user_page(page_table_entry* user_pte) {
    set_pte_attrib(user_pte, PTE_USER);
    alloc_block_resp user_block_alloc_resp = alloc_block();
    if (user_block_alloc_resp.status == ALLOC_BLOCK_SUCCESS) {
        set_pte_attrib(user_pte, PTE_PRESENT);
        set_pte_frame_base_addr(user_pte, user_block_alloc_resp.buffer_base_addr);
    } else if (user_block_alloc_resp.status == ALLOC_BLOCK_ALL_BLOCKS_IN_USE) {
    } else {
        return ALLOC_USER_PAGE_FAILED_ALLOC_BLOCK;
    }

    return ALLOC_USER_PAGE_SUCCESS;
}

typedef enum map_all_pages_status {
    MAP_ALL_PAGES_FAILED_GETTING_PT_BASE_ADDR = 0,
    MAP_ALL_PAGES_UNKNOWN_VIRT_ADDR = 1,
    MAP_ALL_PAGES_SUCCESS = 2,
    MAP_ALL_PAGES_FAILED_PAGE_MEM_MAPPED_IO_CHECK = 3,
    MAP_ALL_PAGES_MEMORY_NEEDS_CONFIG = 4,
    MAP_ALL_PAGES_FAILED_ALLOC_KERNEL_PAGE = 5,
    MAP_ALL_PAGES_FAILED_ALLOC_IO_PAGE = 6,
    MAP_ALL_PAGES_FAILED_ALLOC_USER_PAGE = 7
} map_all_pages_status;

#define NUM_PAGES ((MAX_MEM_ADDR_32_BIT / BYTES_PER_PAGE) + 1)

map_all_pages_status map_all_virt_addr() {
    if (phys_mem_needs_config()) {
        return MAP_ALL_PAGES_MEMORY_NEEDS_CONFIG;
    }

    unsigned int page_num;
    virtual_address curr_virt_addr;
    for (curr_virt_addr = MIN_32_BIT_VIRT_MEM_ADDR, page_num = 0;
         page_num < NUM_PAGES;
         curr_virt_addr += BYTES_PER_PAGE, page_num += 1) {

        page_dir_entry* pde_of_va = get_page_dir_entry(&default_pd, curr_virt_addr);
        pt_base_addr_resp pt_of_va_resp = get_page_table_base_addr(pde_of_va);
        if (pt_of_va_resp.status == PT_BASE_ADDR_SUCCESS) {
        } else {
            return MAP_ALL_PAGES_FAILED_GETTING_PT_BASE_ADDR;
        }

        page_table_entry* pte_of_va = \
            get_page_table_entry((page_table*) pt_of_va_resp.pt_base_addr, curr_virt_addr);

        page_has_mem_map_io_resp curr_va_page_has_mem_io_resp = page_has_mem_map_io(curr_virt_addr);
        if (curr_va_page_has_mem_io_resp.status == PAGE_HAS_MEM_MAP_IO_SUCCESS) {
        } else {
            return MAP_ALL_PAGES_FAILED_PAGE_MEM_MAPPED_IO_CHECK;
        }

        if (curr_va_page_has_mem_io_resp.has_mem_map_io) {
            alloc_io_page_status curr_io_va_alloc_page_status = \
                alloc_io_page(pte_of_va, curr_virt_addr);
            if (curr_io_va_alloc_page_status == ALLOC_IO_PAGE_SUCCESS) {
            } else {
                return MAP_ALL_PAGES_FAILED_ALLOC_IO_PAGE;
            }
        } else if (is_kernel_virt_addr(curr_virt_addr)) {
            alloc_kernel_page_status curr_kernel_page_alloc_status = \
                alloc_kernel_page(pte_of_va, curr_virt_addr);
            if (curr_kernel_page_alloc_status == ALLOC_KERNEL_PAGE_SUCCESS) {
            } else {
                return MAP_ALL_PAGES_FAILED_ALLOC_KERNEL_PAGE;
            }
        } else if (is_user_virt_addr(curr_virt_addr)) {
            alloc_user_page_status curr_user_pa_alloc_status = alloc_user_page(pte_of_va);
            if (curr_user_pa_alloc_status == ALLOC_USER_PAGE_SUCCESS) {
            } else {
                return MAP_ALL_PAGES_FAILED_ALLOC_USER_PAGE;
            }
        } else {
            return MAP_ALL_PAGES_UNKNOWN_VIRT_ADDR;
        }
    }

    return MAP_ALL_PAGES_SUCCESS;
}

init_vm_status init_virtual_mem() {
    assign_all_pt_addr();

    map_all_pages_status initial_mapping_status = map_all_virt_addr();
    if (initial_mapping_status == MAP_ALL_PAGES_SUCCESS) {
    } else {
        return INIT_VM_FAILED_MAPPING_ALL_VIRT_ADDR;
    }

    load_new_pd_status default_pd_load_status = load_new_pd((physical_address) &default_pd);
    if (default_pd_load_status == LOAD_NEW_PD_SUCCESS) {
    } else {
        return INIT_VM_FAILED_LOADING_PD;
    }

    return INIT_VM_SUCCESS;
}
