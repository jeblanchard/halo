#include "virtual_mem_mgr.h"
#include "kernel/memory/virtual/pte.h"
#include "kernel/memory/physical/physical_mem.h"
#include "stddef.h"
#include "kernel/memory/virtual/pde.h"

alloc_page_resp alloc_page(page_table_entry* entry) {
    block_alloc_resp resp = alloc_block();
    if (resp.status == BLOCK_ALLOC_SUCCESS) {
    } else if (resp.status == NO_FREE_BLOCKS) {
        return NO_MEM_AVAIL;
    } else {
        return GENERAL_FAILURE;
    }

    set_pte_frame_base_addr(entry, (physical_address) resp.buffer);
    add_pte_attrib(entry, PTE_PRESENT);

    return SUCCESS;
}

void free_page(page_table_entry* entry) {
    (void) entry;

    physical_address addr_to_free = get_pte_frame_base(entry);
    free_block(addr_to_free);

    rm_pte_attrib(entry, PTE_PRESENT);
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

extern void load_pdbr_asm(physical_address new_pdbr);

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

load_new_pd_status load_new_pd(physical_address new_pdbr) {
    if (pd_has_been_loaded) {
        return PD_ALREADY_LOADED;
    } else {
        current_pdbr = new_pdbr;
    }

    current_page_dir = (page_dir*) new_pdbr;

    load_pdbr_asm(current_pdbr);

    pd_has_been_loaded = true;

    return LOAD_NEW_PD_SUCCESS;
}

bool pde_is_missing(page_dir_entry* entry) {
    return !(pde_is_present(entry));
}

typedef enum init_pde_status {
    INIT_PDE_SUCCESS = 0,
    INIT_PDE_NOT_ENOUGH_MEM = 1,
    INIT_PDE_MEM_BLOCK_TOO_SMALL = 2,
    INIT_PDE_GENERAL_FAILURE = 3
} init_pde_status;

init_pde_status init_pde(page_dir_entry* pde_to_init) {
    block_alloc_resp resp = alloc_block();
    if (resp.status == BLOCK_ALLOC_SUCCESS) {
    } else if (resp.status == NO_FREE_BLOCKS) {
        return INIT_PDE_NOT_ENOUGH_MEM;
    } else {
        return INIT_PDE_GENERAL_FAILURE;
    }
    
    if (resp.buffer_size < sizeof(page_table)) {
        return INIT_PDE_MEM_BLOCK_TOO_SMALL;
    }

    page_table_entry* new_pt = (page_table_entry*) resp.buffer;
    set_pt_base_addr(pde_to_init, (physical_address) new_pt);

    add_pde_attrib(pde_to_init, PDE_PRESENT);
    add_pde_attrib(pde_to_init, PDE_WRITABLE);

    return INIT_PDE_SUCCESS;
}

map_page_status map_page(physical_address new_page_base_addr, virtual_address virt_addr) {
    unsigned int pd_index = get_page_dir_index(virt_addr);
    page_dir_entry* pde = &current_page_dir -> entries[pd_index];

    if (pde_is_missing(pde)) {

        init_pde_status status = init_pde(pde);

        if (status == INIT_PDE_SUCCESS) {
        } else if (status == INIT_PDE_NOT_ENOUGH_MEM ||
                   status == INIT_PDE_MEM_BLOCK_TOO_SMALL) {
            return MP_NOT_ENOUGH_MEM;
        } else if (status == INIT_PDE_GENERAL_FAILURE) {
            return MP_GENERAL_FAILURE;
        } else {
            return MP_GENERAL_FAILURE;
        }
    }   

    page_table* pt = (page_table*) get_page_table_base_addr(pde);
    page_table_entry* pte = get_page_table_entry(pt, virt_addr);

    set_pte_frame_base_addr(pte, new_page_base_addr);

    add_pte_attrib(pte, PTE_PRESENT);

    return MP_SUCCESS;
}

page_dir new_page_dir() {
    page_dir new_pd = {};
    for (int i = 0; i < ENTRIES_PER_PAGE_DIR; i++) {
        new_pd.entries[i] = new_pde();
    }

    return new_pd;
}

get_phys_addr_resp get_phys_addr(virtual_address virt_addr) {

    // need to change this because this section
    // can easily grab the current page dir variable
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

    page_table* pt = (page_table*) get_page_table_base_addr(pde);
    unsigned int pt_index = get_page_table_index(virt_addr);
    page_table_entry* pte = &pt -> entries[pt_index];

    physical_address frame_addr = get_pte_frame_base(pte);

    unsigned int page_offset = get_frame_offset(virt_addr);

    physical_address translation = frame_addr + page_offset;

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

void clear_vm_init() {
    pd_has_been_loaded = false;
}

page_table new_page_table() {
    page_table new_pt = {};

    for (int i = 0; i < ENTRIES_PER_PAGE_TABLE; i++) {
        new_pt.entries[i] = new_pte();
    }

    return new_pt;
}
