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
    } else if (phys_block_resp.status == NO_FREE_BLOCKS) {
        return ALLOC_PAGE_NO_MEM_AVAIL;
    } else {
        return ALLOC_PAGE_GEN_FAILURE;
    }

    set_pte_frame_base_addr(entry, phys_block_resp.buffer);
    add_pte_attrib(entry, PTE_PRESENT);
    pages_in_use += 1;

    return ALLOC_PAGE_SUCCESS;
}

void free_page(page_table_entry* pte_to_free) {
    physical_address addr_to_free = get_pte_frame_base(pte_to_free);
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
    add_pte_attrib(pte_of_va, PTE_PRESENT);

    return MAP_PAGE_SUCCESS;
}

get_phys_addr_resp get_phys_addr(virtual_address virt_addr) {
    get_curr_pd_resp resp = get_curr_pd();
    if (resp.status == GET_CURR_PD_SUCCESS) {
    } else if (resp.status == PD_INIT_NEEDED) {
        return (get_phys_addr_resp) {
            status: NO_INITIALIZED_PD,
            phys_addr: 0};
    } else {
        return (get_phys_addr_resp) {
            status: GET_PHYS_ADDR_GEN_FAILURE,
            phys_addr: 0};
    }

    unsigned int pd_index = get_page_dir_index(virt_addr);
    page_dir_entry* pde = &resp.curr_pd -> entries[pd_index];

    pt_base_addr_resp pt_resp = get_page_table_base_addr(pde);
    if (pt_resp.status == PT_BASE_ADDR_SUCCESS) {
    } else if (pt_resp.status == PT_DNE) {
        return (get_phys_addr_resp) {
            status: GET_PHYS_ADDR_PT_DNE,
            phys_addr: 0
        };
    } else {
        return (get_phys_addr_resp) {
            status: GET_PHYS_ADDR_FAILED_GET_PT_ADDR,
            phys_addr: 0
        };
    }

    page_table* pt = (page_table*) pt_resp.pt_base_addr;

    unsigned int pt_index = get_page_table_index(virt_addr);
    page_table_entry* pte = &pt -> entries[pt_index];

    physical_address frame_base_addr = get_pte_frame_base(pte);
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

void init_default_pd() {
    default_pd = new_page_dir();
    for (int i = 0; i < ENTRIES_PER_PAGE_DIR; i++) {
        set_pt_base_addr(
            &default_pd.entries[i],
            (physical_address) &all_page_tables[i]);
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

bool is_io_virt_addr(virtual_address virt_addr) {
    if (virt_addr > IO_BASE_VIRT_ADDR &&
        virt_addr <= IO_MAX_VIRT_ADDR) {

        return true;
    }

    return false;
}

bool is_a_user_virt_addr(virtual_address addr) {
    if (addr >= USER_SPACE_BASE_VIRT_ADDR &&
        addr <= USER_SPACE_MAX_VIRT_ADDR) {
            return true;
    }

    return false;
}

bool is_a_kernel_virt_addr(virtual_address addr) {
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

typedef enum kernel_phys_addr_status {
    VA_TO_KERNEL_PA_SUCCESS = 0,
    NOT_A_KERNEL_VIRT_ADDR = 1
} kernel_phys_addr_status;

typedef struct kernel_phys_addr_trans_resp {
    kernel_phys_addr_status status;
    physical_address kernel_phys_addr;
} kernel_phys_addr_trans_resp;

kernel_phys_addr_trans_resp translate_to_kernel_phys_addr(virtual_address virt_addr) {
    if (is_a_kernel_virt_addr(virt_addr)) {
        extern unsigned int KERNEL_PHYS_ADDRESS;
        physical_address kernel_phys_addr = \
            virt_addr - (KERNEL_SPACE_BASE_VIRT_ADDR - KERNEL_PHYS_ADDRESS);

        return (kernel_phys_addr_trans_resp) {
            status: VA_TO_KERNEL_PA_SUCCESS,
            kernel_phys_addr: kernel_phys_addr
        };
    }

    return (kernel_phys_addr_trans_resp) {
        status: NOT_A_KERNEL_VIRT_ADDR,
        kernel_phys_addr: 0
    };
}

typedef enum init_kernel_vm_status {
    INIT_KERNEL_VM_SUCCESS = 0,
    INIT_KERNEL_FAILED_ALLOC_PT = 1,
    INIT_KERNEL_FAILED_ALLOC_SPEC_FRAME = 2,
    INIT_KERNEL_FAILED_TRANS_TO_KERNEL_PA = 3,
    INIT_KERNEL_FAILED_GETTING_PT_BASE_ADDR = 4
} init_kernel_vm_status;

init_kernel_vm_status init_kernel_virtual_mem(page_dir* curr_pd) {
    for (virtual_address kernel_virt_addr = KERNEL_SPACE_BASE_VIRT_ADDR;
         kernel_virt_addr <= KERNEL_SPACE_MAX_VIRT_ADDR;
         kernel_virt_addr += BYTES_PER_PAGE) {

        page_dir_entry* curr_pde = get_page_dir_entry(curr_pd, kernel_virt_addr);
        pt_base_addr_resp curr_pde_pt_resp = get_page_table_base_addr(curr_pde);
        if (curr_pde_pt_resp.status == PT_BASE_ADDR_SUCCESS) {
        } else {
            return INIT_KERNEL_FAILED_GETTING_PT_BASE_ADDR;
        }

        page_table* curr_pde_pt = (page_table*) curr_pde_pt_resp.pt_base_addr;
        page_table_entry* curr_pte = get_page_table_entry(curr_pde_pt, kernel_virt_addr);

        add_pde_attrib(curr_pde, PDE_PRESENT);
        add_pte_attrib(curr_pte, PTE_PRESENT);

        kernel_phys_addr_trans_resp translation_resp = translate_to_kernel_phys_addr(kernel_virt_addr);
        if (translation_resp.status == VA_TO_KERNEL_PA_SUCCESS) {
        } else {
            return INIT_KERNEL_FAILED_TRANS_TO_KERNEL_PA;
        }

        alloc_spec_frame_resp resp = alloc_spec_frame(translation_resp.kernel_phys_addr);
        if (resp.status == ALLOC_SPEC_FRAME_SUCCESS) {
            set_pte_frame_base_addr(curr_pte, resp.buffer);
        } else {
            return INIT_KERNEL_FAILED_ALLOC_SPEC_FRAME;
        }
    }

    return INIT_KERNEL_VM_SUCCESS;
}

typedef enum io_phys_addr_status {
    VA_TO_IO_PA_SUCCESS = 0,
    NOT_IO_VIRT_ADDR = 1
} io_phys_addr_status;

typedef struct io_phys_addr_resp {
    io_phys_addr_status status;
    physical_address io_phys_addr;
} io_phys_addr_resp;

io_phys_addr_resp translate_to_io_phys_addr(virtual_address virt_addr) {
    if (is_io_virt_addr(virt_addr)) {
        physical_address io_phys_addr = (physical_address) virt_addr;

        return (io_phys_addr_resp) {
            status: VA_TO_IO_PA_SUCCESS,
            io_phys_addr: io_phys_addr
        };
    }

    return (io_phys_addr_resp) {
        status: NOT_IO_VIRT_ADDR,
        io_phys_addr: 0
    };
}

typedef enum init_io_vm_status {
    INIT_IO_VM_SUCCESS = 0,
    INIT_IO_VM_FAILED_ALLOC_PT = 1,
    INIT_IO_FAILED_TRANS_TO_IO_PA = 2,
    INIT_IO_FAILED_ALLOC_SPEC_FRAME = 3,
    INIT_IO_FAILED_GETTING_PT_BASE_ADDR = 4
} init_io_vm_status;

init_io_vm_status init_io_virtual_mem(page_dir* curr_pd) {
    for (virtual_address io_virt_addr = IO_BASE_VIRT_ADDR;
         io_virt_addr <= IO_MAX_VIRT_ADDR;
         io_virt_addr += BYTES_PER_PAGE) {

        page_dir_entry* curr_pde = get_page_dir_entry(curr_pd, io_virt_addr);

        pt_base_addr_resp curr_pt_resp = get_page_table_base_addr(curr_pde);
        if (curr_pt_resp.status == PT_BASE_ADDR_SUCCESS) {
        } else {
            return INIT_IO_FAILED_GETTING_PT_BASE_ADDR;
        }

        page_table* curr_pt = (page_table*) curr_pt_resp.pt_base_addr;
        page_table_entry* curr_pte = get_page_table_entry(curr_pt, io_virt_addr);

        add_pde_attrib(curr_pde, PDE_PRESENT);
        add_pte_attrib(curr_pte, PTE_PRESENT);

        io_phys_addr_resp translation_resp = translate_to_io_phys_addr(io_virt_addr);
        if (translation_resp.status == VA_TO_IO_PA_SUCCESS) {
        } else {
            return INIT_IO_FAILED_TRANS_TO_IO_PA;
        }

        alloc_spec_frame_resp resp = alloc_spec_frame(translation_resp.io_phys_addr);
        if (resp.status == ALLOC_SPEC_FRAME_SUCCESS) {
            set_pte_frame_base_addr(curr_pte, resp.buffer);
        } else {
            return INIT_IO_FAILED_ALLOC_SPEC_FRAME;
        }
    }

    return INIT_IO_VM_SUCCESS;
}

typedef enum user_phys_addr_status {
    VA_TO_USER_PA_SUCCESS = 0,
    NOT_USER_VIRT_ADDR = 1
} user_phys_addr_status;

typedef struct user_phys_addr_resp {
    user_phys_addr_status status;
    physical_address user_phys_addr;
} user_phys_addr_resp;

user_phys_addr_resp translate_to_user_phys_addr(virtual_address virt_addr) {
    if (is_a_user_virt_addr(virt_addr)) {
        physical_address user_phys_addr = (physical_address) virt_addr;

        return (user_phys_addr_resp) {
            status: VA_TO_USER_PA_SUCCESS,
            user_phys_addr: user_phys_addr
        };
    }

    return (user_phys_addr_resp) {
        status: NOT_USER_VIRT_ADDR,
        user_phys_addr: 0
    };
}

typedef enum init_user_vm_status {
    INIT_USER_VM_SUCCESS = 0,
    INIT_USER_VM_FAILED_ALLOC_PT = 1,
    INIT_USER_FAILED_TRANS_TO_USER_PA = 2,
    INIT_USER_FAILED_ALLOC_SPEC_FRAME = 3,
    INIT_USER_FAILED_GETTING_PT_BASE_ADDR = 4
} init_user_vm_status;

init_user_vm_status init_user_virtual_mem(page_dir* curr_pd) {
    for (virtual_address user_virt_addr = USER_SPACE_BASE_VIRT_ADDR;
         user_virt_addr <= USER_SPACE_MAX_VIRT_ADDR;
         user_virt_addr += BYTES_PER_PAGE) {

        page_dir_entry* curr_pde = get_page_dir_entry(curr_pd, user_virt_addr);
        pt_base_addr_resp curr_pt_resp = get_page_table_base_addr(curr_pde);
        if (curr_pt_resp.status == PT_BASE_ADDR_SUCCESS) {
        } else {
            return INIT_USER_FAILED_GETTING_PT_BASE_ADDR;
        }

        page_table* curr_pt = (page_table*) curr_pt_resp.pt_base_addr;
        page_table_entry* curr_pte = get_page_table_entry(curr_pt, user_virt_addr);

        add_pde_attrib(curr_pde, PDE_PRESENT);
        add_pte_attrib(curr_pte, PTE_PRESENT);

        user_phys_addr_resp translation_resp = translate_to_user_phys_addr(user_virt_addr);
        if (translation_resp.status == VA_TO_USER_PA_SUCCESS) {
        } else {
            return INIT_USER_FAILED_TRANS_TO_USER_PA;
        }

        alloc_spec_frame_resp resp = alloc_spec_frame(translation_resp.user_phys_addr);
        if (resp.status == ALLOC_SPEC_FRAME_SUCCESS) {
            set_pte_frame_base_addr(curr_pte, resp.buffer);
        } else {
            return INIT_USER_FAILED_ALLOC_SPEC_FRAME;
        }
    }

    return INIT_USER_VM_SUCCESS;
}

init_vm_status init_virtual_mem() {
    default_pd = new_page_dir();

    init_io_vm_status io_space_status = init_io_virtual_mem(&default_pd);
    if (io_space_status == INIT_IO_VM_SUCCESS) {
    } else {
        free_pd(&default_pd);
        return INIT_VM_FAILED_IO_SPACE;
    }

    init_kernel_vm_status kernel_space_status = init_kernel_virtual_mem(&default_pd);
    if (kernel_space_status == INIT_KERNEL_VM_SUCCESS) {
    } else {
        free_pd(&default_pd);
        return INIT_VM_FAILED_KERNEL_SPACE;
    }

    init_user_vm_status user_space_status = init_user_virtual_mem(&default_pd);
    if (user_space_status == INIT_USER_VM_SUCCESS) {
    } else {
        free_pd(&default_pd);
        return INIT_VM_FAILED_USER_SPACE;
    }

    return INIT_VM_SUCCESS;
}
