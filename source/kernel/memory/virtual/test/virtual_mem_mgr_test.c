#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel/memory/virtual/pte.h"
#include "kernel/memory/virtual/virtual_mem_mgr.h"
#include "kernel/memory/virtual/pde/pde.h"

alloc_block_resp __wrap_alloc_block() {
    alloc_block_resp* resp = mock_ptr_type(alloc_block_resp*);
    return *resp;
}

page_table_entry entry;

static int alloc_page_test_teardown(void **state) {
    (void) state;

    free_page(&entry);
    entry = new_pte();

    return 0;
}

static void alloc_page_success_test(void **state) {
    (void) state;

    physical_address buffer_base_addr = 5 * BYTES_PER_PAGE;
    alloc_block_resp block_resp = {
        status: ALLOC_BLOCK_SUCCESS,
        buffer_size: BYTES_PER_MEMORY_BLOCK,
        buffer_base_addr: buffer_base_addr
    };

    will_return(__wrap_alloc_block, (uintptr_t) &block_resp);

    entry = new_pte();

    alloc_page_status status = alloc_page(&entry);

    assert_true(status == ALLOC_PAGE_SUCCESS);
    assert_true(page_is_present(&entry));
    assert_true(get_pte_frame_base_addr(&entry) == buffer_base_addr);
}

static void alloc_page_no_mem_avail_test(void **state) {
    (void) state;

    entry = new_pte();

    alloc_block_resp no_mem_alloc_block_resp = {
        status: ALLOC_BLOCK_ALL_BLOCKS_IN_USE,
        buffer_size: 0,
        buffer_base_addr: 0
    };
    
    will_return(__wrap_alloc_block, (uintptr_t) &no_mem_alloc_block_resp);

    alloc_page_status status = alloc_page(&entry);

    assert_true(status == ALLOC_PAGE_PHYS_MEM_IS_FULL);
    assert_true(page_is_present(&entry) == false);
}

static void alloc_page_failed_setting_pte_frame_test(void **state) {
    (void) state;

    entry = new_pte();

    physical_address non_aligned_buffer_addr = 3;
    alloc_block_resp unaligned_block_resp = {
        status: ALLOC_BLOCK_SUCCESS,
        buffer_size: BYTES_PER_MEMORY_BLOCK,
        buffer_base_addr: non_aligned_buffer_addr
    };
    
    will_return(__wrap_alloc_block, (uintptr_t) &unaligned_block_resp);

    alloc_page_status status = alloc_page(&entry);

    assert_true(status == ALLOC_PAGE_FAILED_SETTING_PTE_FRAME);
    assert_true(page_is_missing(&entry));
}

static void get_pages_in_use_after_page_alloc_test(void **state) {
    (void) state;

    page_table_entry entry = new_pte();

    alloc_block_resp non_zero_buff_addr_resp = {
        status: ALLOC_BLOCK_SUCCESS,
        buffer_size: BYTES_PER_MEMORY_BLOCK,
        buffer_base_addr: 0
    };

    will_return(__wrap_alloc_block, (uintptr_t) &non_zero_buff_addr_resp);

    unsigned int og_pages_in_use = get_num_pages_in_use();

    alloc_page_status status = alloc_page(&entry);
    assert_true(status == ALLOC_PAGE_SUCCESS);

    unsigned int new_pages_in_use = get_num_pages_in_use();

    assert_true(new_pages_in_use == og_pages_in_use + 1);
}

bool correct_block_addr_was_freed = false;
page_table_entry entry_of_page_to_free;
physical_address fake_frame_base_addr = 3 * BYTES_PER_PAGE;

void __wrap_free_block(physical_address block_base_addr) {
    if (fake_frame_base_addr == block_base_addr) {
        correct_block_addr_was_freed = true;
    }
}

static void free_page_test(void **state) {
    (void) state;

    entry_of_page_to_free = new_pte();
    alloc_block_resp mocked_alloc_block_resp = {
        status: ALLOC_BLOCK_SUCCESS,
        buffer_base_addr: fake_frame_base_addr,
        buffer_size: BYTES_PER_MEMORY_BLOCK
    };

    will_return(__wrap_alloc_block, (uintptr_t) &mocked_alloc_block_resp);
    alloc_page(&entry_of_page_to_free);
    
    unsigned int num_pages_in_use_after_alloc = get_num_pages_in_use();

    free_page(&entry_of_page_to_free);
    unsigned int num_pages_in_use_after_free = get_num_pages_in_use();

    assert_true(page_is_missing(&entry_of_page_to_free));
    assert_true(correct_block_addr_was_freed);
    assert_true(num_pages_in_use_after_alloc - 1 \
        == num_pages_in_use_after_free);
}

static void get_page_table_index_test(void **state) {
    (void) state;

    virtual_address v_addr = 0x12345678;

    unsigned int index = get_page_table_index(v_addr);

    unsigned int correct_index = (v_addr >> 12) & 0x3ff;

    assert_true(index == correct_index);
}

static void get_page_table_entry_test(void **state) {
    (void) state;

    page_table fake_page_table = {};

    fake_page_table.entries[0] = (page_table_entry) 0x12345678;
    fake_page_table.entries[1] = (page_table_entry) 0x91011121;
    fake_page_table.entries[2] = (page_table_entry) 0x13141516;

    virtual_address entry_2_virt_addr = 0x1000;

    page_table_entry* actual_pte = \
        get_page_table_entry(&fake_page_table, entry_2_virt_addr);
 
    assert_ptr_equal(actual_pte, &fake_page_table.entries[1]);
}

static void get_page_dir_index_test(void **state) {
    (void) state;

    virtual_address v_addr = 0x12345678;

    unsigned int index = get_page_dir_index(v_addr);

    unsigned int correct_index = (v_addr >> 22) & 0x3ff;

    assert_true(index == correct_index);
}

static void get_page_dir_entry_test(void **state) {
    (void) state;

    page_dir fake_page_dir = {};

    fake_page_dir.entries[0] = (page_dir_entry) 0x12345678;
    fake_page_dir.entries[1] = (page_dir_entry) 0x91011121;
    fake_page_dir.entries[2] = (page_dir_entry) 0x13141516;

    virtual_address entry_2_virt_addr = 0x400000;

    page_dir_entry* actual_pde = \
        get_page_dir_entry(&fake_page_dir, entry_2_virt_addr);
 
    assert_ptr_equal(actual_pde, &fake_page_dir.entries[1]);
}

void __wrap_load_pdbr_asm(physical_address new_pdbr_base_addr) {
    (void) new_pdbr_base_addr;
}

static void load_new_pd_test(void **state) {
    (void) state;

    page_dir fake_page_dir = new_page_dir();
    fake_page_dir.entries[0] = (page_dir_entry) 0x12345678;
    fake_page_dir.entries[1] = (page_dir_entry) 0x91011121;
    fake_page_dir.entries[2] = (page_dir_entry) 0x13141516;

    load_new_pd((physical_address) &fake_page_dir);

    get_curr_pdbr_resp curr_pdbr_resp = get_curr_pdbr();

    assert_ptr_equal((physical_address) &fake_page_dir, curr_pdbr_resp.curr_pdbr);
}

static int load_new_pd_test_teardown(void **state) {
    (void) state;

    clear_vm_config();

    return 0;
}

static void get_curr_pdbr_test(void **state) {
    (void) state;

    page_dir fake_pd = new_page_dir();

    load_new_pd((physical_address) &fake_pd);

    assert_true((physical_address) &fake_pd == get_curr_pdbr().curr_pdbr);
}

static int get_curr_pdbr_test_teardown(void **state) {
    (void) state;

    clear_vm_config();

    return 0;
}

static void get_curr_pd_test(void **state) {
    (void) state;

    page_dir fake_pd = new_page_dir();
    load_new_pd((physical_address) &fake_pd);

    get_curr_pd_resp resp = get_curr_pd();

    assert_true(resp.status == GET_CURR_PD_SUCCESS);
    assert_true(&fake_pd == resp.curr_pd);
}

static int get_curr_pd_test_teardown(void **state) {
    (void) state;

    clear_vm_config();

    return 0;
}

static void get_curr_pd_before_pd_init_test(void **state) {
    (void) state;

    clear_vm_config();

    get_curr_pd_resp resp = get_curr_pd();

    assert_true(resp.status == PD_INIT_NEEDED);
    assert_true(resp.curr_pd == NULL);
}

static void clear_vm_init_test(void **state) {
    (void) state;

    clear_vm_config();

    get_curr_pd_resp resp = get_curr_pd();

    assert_true(resp.status == PD_INIT_NEEDED);
}

static void flush_tlb_entry_test(void **state) {
    (void) state;
    assert_true(true);
}

static void map_page_base_addr_success_test(void **state) {
    (void) state;

    unsigned int pd_index = 1;
    unsigned int pt_index = 4;
    unsigned int page_offset = 300;

    new_virt_addr_resp new_va_resp = new_virt_addr(pd_index, pt_index, page_offset);
    assert_true(new_va_resp.status == NEW_VIRT_ADDR_SUCCESS);

    page_dir pd = new_page_dir();
    page_table pt = new_page_table();
    physical_address frame_base_addr = 3 * BYTES_PER_PAGE;
    
    set_pt_base_addr(&pd.entries[pd_index], (physical_address) &pt);
    set_pde_attrib(&pd.entries[pd_index], PDE_PRESENT);

    set_pte_frame_base_addr_status initial_frame_set_status = \
        set_pte_frame_base_addr(&pt.entries[pt_index], (physical_address) frame_base_addr);
    assert_true(initial_frame_set_status == SET_PTE_FRAME_BASE_ADDR_SUCCESS);

    set_pte_attrib(&pt.entries[pt_index], PTE_PRESENT);

    load_new_pd_status new_pd_status = load_new_pd((physical_address) &pd);
    assert_true(new_pd_status == LOAD_NEW_PD_SUCCESS);

    get_phys_addr_resp og_phys_addr_resp = get_phys_addr(new_va_resp.virt_addr);
    assert_true(og_phys_addr_resp.status == GET_PHYS_ADDR_SUCCESS);

    physical_address new_frame_base_addr = 10 * BYTES_PER_PAGE;

    map_page_status mp_status = map_page_base_addr(new_frame_base_addr, new_va_resp.virt_addr);
    assert_true(mp_status == MAP_PAGE_SUCCESS);

    get_phys_addr_resp new_phys_addr_resp = get_phys_addr(new_va_resp.virt_addr);
    assert_true(new_phys_addr_resp.status == GET_PHYS_ADDR_SUCCESS);

    physical_address correct_new_phys_addr = new_frame_base_addr + page_offset;
    assert_true(new_phys_addr_resp.phys_addr == correct_new_phys_addr);
}

static int map_page_base_addr_teardown(void **state) {
    (void) state;

    clear_vm_config();

    return 0;
}

static void new_virt_addr_test(void **state) {
    (void) state;

    unsigned int pd_index = 1;
    unsigned int pt_index = 4;
    unsigned int page_offset = 300;

    new_virt_addr_resp new_va_resp = \
        new_virt_addr(pd_index, pt_index, page_offset);

    virtual_address correct_virt_addr = 0x0040412c;

    assert_true(new_va_resp.status == NEW_VIRT_ADDR_SUCCESS);
    assert_true(new_va_resp.virt_addr = correct_virt_addr);
}

static void get_phys_addr_test(void **state) {
    (void) state;

    unsigned int pd_index = 1;
    unsigned int pt_index = 4;
    unsigned int page_offset = 300;

    new_virt_addr_resp new_va_resp = \
        new_virt_addr(pd_index, pt_index, page_offset);
    assert_true(new_va_resp.status == NEW_VIRT_ADDR_SUCCESS);

    page_dir pd = new_page_dir();
    page_table pt = new_page_table();
    set_pt_base_addr(&pd.entries[pd_index], (physical_address) &pt);

    physical_address some_frame_base_addr = 3 * BYTES_PER_PAGE;
    set_pte_frame_base_addr(&pt.entries[pt_index], some_frame_base_addr);

    load_new_pd((physical_address) &pd);

    get_phys_addr_resp phys_addr_resp = get_phys_addr(new_va_resp.virt_addr);
    assert_true(phys_addr_resp.status == GET_PHYS_ADDR_SUCCESS);

    physical_address correct_phys_addr = some_frame_base_addr + page_offset;

    assert_true(correct_phys_addr == phys_addr_resp.phys_addr);
}

static int get_phys_addr_teardown(void **state) {
    (void) state;

    clear_vm_config();

    return 0;
}

static void new_page_dir_test(void **state) {
    (void) state;

    page_dir new_pd = new_page_dir();

    for (int i = 0; i < ENTRIES_PER_PAGE_DIR; i++) {
        assert_true(new_pd.entries[i] == new_pde());
    }
}

static void get_frame_offset_test(void **state) {
    (void) state;

    unsigned int frame_offset = 23;

    new_virt_addr_resp new_va_resp = new_virt_addr(0, 0, frame_offset);
    assert_true(new_va_resp.status == NEW_VIRT_ADDR_SUCCESS);

    assert_true(get_frame_offset(new_va_resp.virt_addr) == frame_offset);
}

page_dir* curr_pd;

bool __wrap_phys_mem_needs_config() {
    return (bool) mock();
}

bool in_init_vm_user_space_test = false;

mem_mapped_io_resp __wrap_frame_has_mem_mapped_io(physical_address potential_io_addr) {
    (void) potential_io_addr;

    mem_mapped_io_resp* mocked_resp = mock_ptr_type(mem_mapped_io_resp*);

    return *mocked_resp;
}

alloc_spec_frame_resp __wrap_alloc_spec_frame(physical_address frame_base_addr) {
    (void) frame_base_addr;

    return (alloc_spec_frame_resp) {
        status: ALLOC_SPEC_FRAME_SUCCESS,
        buffer_size: BYTES_PER_MEMORY_BLOCK,
        buffer_base_addr: frame_base_addr
    };
}

static int init_vm_test_teardown(void **state) {
    (void) state;

    clear_vm_config();

    return 0;
}

static void init_vm_success_test(void **state) {
    (void) state;

    will_return(__wrap_phys_mem_needs_config, false);

    physical_address some_block_base_addr = 3 * BYTES_PER_MEMORY_BLOCK;
    alloc_block_resp successful_alloc_block_resp = {
        status: ALLOC_BLOCK_SUCCESS,
        buffer_size: BYTES_PER_MEMORY_BLOCK,
        buffer_base_addr: some_block_base_addr
    };

    will_return_always(__wrap_alloc_block, (uintptr_t) &successful_alloc_block_resp);

    mem_mapped_io_resp no_mem_map_io_resp = {
        status: MEM_MAPPED_IO_SUCCESS,
        has_mem_mapped_io: false
    };

    will_return_always(__wrap_frame_has_mem_mapped_io, (uintptr_t) &no_mem_map_io_resp);

    init_vm_status status = init_virtual_mem();
    assert_true(status == INIT_VM_SUCCESS);
}

static void init_vm_kernel_space_test(void **state) {
    (void) state;

    will_return(__wrap_phys_mem_needs_config, false);

    mem_mapped_io_resp no_mem_map_io_resp = {
        status: MEM_MAPPED_IO_SUCCESS,
        has_mem_mapped_io: false};

    will_return_always(__wrap_frame_has_mem_mapped_io, (uintptr_t) &no_mem_map_io_resp);

    physical_address some_block_base_addr = 3 * BYTES_PER_MEMORY_BLOCK;
    alloc_block_resp successful_alloc_block_resp = {
        status: ALLOC_BLOCK_SUCCESS,
        buffer_size: BYTES_PER_MEMORY_BLOCK,
        buffer_base_addr: some_block_base_addr
    };

    will_return_always(__wrap_alloc_block, (uintptr_t) &successful_alloc_block_resp);

    init_vm_status status = init_virtual_mem();
    assert_true(status == INIT_VM_SUCCESS);

    unsigned int kernel_va_space_length = \
        KERNEL_SPACE_MAX_VIRT_ADDR - KERNEL_SPACE_BASE_VIRT_ADDR;
    unsigned int sections_in_kernel_va_space = kernel_va_space_length / BYTES_PER_PAGE;

    virtual_address kernel_va;
    unsigned int section_num;
    for (kernel_va = KERNEL_SPACE_BASE_VIRT_ADDR, section_num = 0;
         section_num < sections_in_kernel_va_space;
         kernel_va += BYTES_PER_PAGE, section_num += 1) {

        get_phys_addr_resp actual_kernel_phys_addr_resp = get_phys_addr(kernel_va);
        assert_true(actual_kernel_phys_addr_resp.status == GET_PHYS_ADDR_SUCCESS);

        kernel_phys_addr_trans_resp correct_kernel_phys_addr_resp = \
            trans_to_kernel_phys_addr(kernel_va);
        assert_true(correct_kernel_phys_addr_resp.status == TRANS_TO_KERNEL_PA_SUCCESS);

        if (actual_kernel_phys_addr_resp.phys_addr != \
            correct_kernel_phys_addr_resp.kernel_phys_addr) {
        }

        assert_true(actual_kernel_phys_addr_resp.phys_addr == \
            correct_kernel_phys_addr_resp.kernel_phys_addr);
    }
}

static void init_vm_user_space_test(void **state) {
    (void) state;

    will_return(__wrap_phys_mem_needs_config, false);

    mem_mapped_io_resp no_mem_map_io_resp = {
        status: MEM_MAPPED_IO_SUCCESS,
        has_mem_mapped_io: false};

    will_return_always(__wrap_frame_has_mem_mapped_io, (uintptr_t) &no_mem_map_io_resp);

    physical_address some_block_base_addr = 3 * BYTES_PER_MEMORY_BLOCK;
    alloc_block_resp successful_alloc_block_resp = {
        status: ALLOC_BLOCK_SUCCESS,
        buffer_size: BYTES_PER_MEMORY_BLOCK,
        buffer_base_addr: some_block_base_addr
    };

    will_return_always(__wrap_alloc_block, (uintptr_t) &successful_alloc_block_resp);

    init_vm_status status = init_virtual_mem();
    assert_true(status == INIT_VM_SUCCESS);

    unsigned int user_va_space_length = \
        USER_SPACE_MAX_VIRT_ADDR - USER_SPACE_BASE_VIRT_ADDR;
    unsigned int sections_in_user_va_space = user_va_space_length / BYTES_PER_PAGE;

    virtual_address user_va;
    unsigned int section_num;
    for (user_va = USER_SPACE_BASE_VIRT_ADDR, section_num = 0;
         section_num < sections_in_user_va_space;
         user_va += BYTES_PER_PAGE, section_num += 1) {

        get_phys_addr_resp actual_user_phys_addr_resp = get_phys_addr(user_va);
        assert_true(actual_user_phys_addr_resp.status == GET_PHYS_ADDR_SUCCESS);

        assert_true(actual_user_phys_addr_resp.phys_addr == some_block_base_addr);
    }
}

static void trans_to_kernel_phys_addr_test(void **state) {
    (void) state;

    unsigned int kernel_va_space_length = \
        KERNEL_SPACE_MAX_VIRT_ADDR - KERNEL_SPACE_BASE_VIRT_ADDR;
    unsigned int sections_in_kernel_va_space = kernel_va_space_length / BYTES_PER_PAGE;

    extern physical_address KERNEL_PHYS_ADDRESS;
    virtual_address kernel_va;
    unsigned int section_num;
    for (kernel_va = KERNEL_SPACE_BASE_VIRT_ADDR, section_num = 0;
         section_num < sections_in_kernel_va_space;
         kernel_va += BYTES_PER_PAGE, section_num += 1) {
            
        kernel_phys_addr_trans_resp actual_kernel_phys_addr_resp = \
            trans_to_kernel_phys_addr(kernel_va);
        assert_true(actual_kernel_phys_addr_resp.status == TRANS_TO_KERNEL_PA_SUCCESS);

        physical_address correct_kernel_phys_addr = \
            kernel_va - (KERNEL_SPACE_BASE_VIRT_ADDR - KERNEL_PHYS_ADDRESS);

        assert_true(actual_kernel_phys_addr_resp.kernel_phys_addr == correct_kernel_phys_addr);
    }
}

static void trans_to_kernel_phys_addr_not_kernel_addr_test(void **state) {
    (void) state;

    virtual_address non_kernel_va = USER_SPACE_BASE_VIRT_ADDR;

    kernel_phys_addr_trans_resp actual_kernel_phys_addr_resp = \
        trans_to_kernel_phys_addr(non_kernel_va);

    assert_true(actual_kernel_phys_addr_resp.status == NOT_A_KERNEL_VIRT_ADDR);
}

static void init_vm_failed_mapping_all_pages_test(void **state) {
    (void) state;

    alloc_block_resp alloc_block_failure_resp = {
        status: ALLOC_BLOCK_ALL_BLOCKS_IN_USE,
        buffer_size: 0,
        buffer_base_addr: 0
    };

    will_return_always(__wrap_alloc_block, (uintptr_t) &alloc_block_failure_resp);

    will_return(__wrap_phys_mem_needs_config, false);

    mem_mapped_io_resp no_mem_map_io_resp = {
        status: MEM_MAPPED_IO_SUCCESS,
        has_mem_mapped_io: false
    };

    will_return_always(__wrap_frame_has_mem_mapped_io, (uintptr_t) &no_mem_map_io_resp);

    init_vm_status status = init_virtual_mem();
    assert_true(status == INIT_VM_FAILED_MAPPING_ALL_VIRT_ADDR);
}

static void init_vm_failed_loading_pd_test(void **state) {
    (void) state;

    will_return(__wrap_phys_mem_needs_config, false);

    mem_mapped_io_resp no_mem_map_io_resp = {
        status: MEM_MAPPED_IO_SUCCESS,
        has_mem_mapped_io: false
    };

    will_return_always(__wrap_frame_has_mem_mapped_io, (uintptr_t) &no_mem_map_io_resp);

    physical_address some_buffer_base_addr = 0x1234;
    alloc_block_resp alloc_block_success_resp = {
        status: ALLOC_BLOCK_SUCCESS,
        buffer_size: BYTES_PER_MEMORY_BLOCK,
        buffer_base_addr: some_buffer_base_addr
    };

    will_return_always(__wrap_alloc_block, (uintptr_t) &alloc_block_success_resp);

    physical_address some_pd_addr = 0x1234;
    load_new_pd(some_pd_addr);

    init_vm_status status = init_virtual_mem();
    assert_true(status == INIT_VM_FAILED_LOADING_PD);
}

static int free_pd_test_teardown(void **state) {
    (void) state;

    clear_vm_config();

    return 0;
}

static void free_pd_success_test(void **state) {
    (void) state;

    page_dir pd = new_page_dir();

    page_table pt_being_alloced = new_page_table();
    unsigned int pd_index_of_pt_being_alloced = 2;
    set_pt_base_addr(&pd.entries[pd_index_of_pt_being_alloced],
        (physical_address) &pt_being_alloced);

    page_table meaningless_pt = new_page_table();
    for (unsigned int i = 0; i < ENTRIES_PER_PAGE_TABLE; i++) {
        if (i == pd_index_of_pt_being_alloced) {
            continue;
        }

        set_pt_base_addr(&pd.entries[i],
            (physical_address) &meaningless_pt);
    }

    unsigned int num_pages_to_alloc = 5;
    physical_address frame_to_free_base_addr = 2 * BYTES_PER_MEMORY_BLOCK;
    for (unsigned int i = 0; i < num_pages_to_alloc; i++) {
        alloc_block_resp good_alloc_block_resp = {
            status: ALLOC_BLOCK_SUCCESS,
            buffer_base_addr: frame_to_free_base_addr + i * BYTES_PER_MEMORY_BLOCK,
            buffer_size: BYTES_PER_MEMORY_BLOCK
        };

        will_return(__wrap_alloc_block, (uintptr_t) &good_alloc_block_resp);
        alloc_page_status fake_page_alloc_status = alloc_page(&pt_being_alloced.entries[i]);
        assert_true(fake_page_alloc_status == ALLOC_PAGE_SUCCESS);
    }

    unsigned pages_in_use_before_free_pd = get_num_pages_in_use();
    
    free_pd_status free_test_pd_status = free_pd(&pd);
    assert_true(free_test_pd_status == FREE_PD_SUCCESS);

    unsigned pages_in_use_after_free_pd = get_num_pages_in_use();
    assert_true(pages_in_use_after_free_pd == \
        pages_in_use_before_free_pd - num_pages_to_alloc);
}

static void free_pd_failed_getting_pt_test(void **state) {
    (void) state;

    page_dir pd = new_page_dir();
    
    free_pd_status free_test_pd_status = free_pd(&pd);

    assert_true(free_test_pd_status == FREE_PD_FAILED_GETTING_PT);
}

static void pd_4kb_aligned_test(void **state) {
    (void) state;

    page_dir pd = new_page_dir();
    assert_true((unsigned int) &pd % 4096 == 0);
}

static void page_table_4kb_aligned_test(void **state) {
    (void) state;

    page_table pt = new_page_table();
    assert_true((unsigned int) &pt % 4096 == 0);
}

static void map_page_failed_getting_pt_test(void **state) {
    (void) state;

    page_dir pd = new_page_dir();
    virtual_address some_virt_addr = 0x456;
    unsigned int va_pd_index = get_page_dir_index(some_virt_addr);
    set_pde_attrib(&pd.entries[va_pd_index], PDE_PRESENT);

    load_new_pd_status new_pd_status = load_new_pd((physical_address) &pd);
    assert_true(new_pd_status == LOAD_NEW_PD_SUCCESS);

    physical_address some_phys_addr = 0x123;
    map_page_status failed_mapping_status = \
        map_page_base_addr(some_phys_addr, some_virt_addr);

    assert_true(failed_mapping_status == MAP_PAGE_FAILED_GETTING_PT_BASE_ADDR);
}

static int map_page_failed_getting_pt_test_teardown(void **state) {
    (void) state;

    clear_vm_config();

    return 0;
}

static void get_phys_addr_pt_dne_test(void **state) {
    (void) state;

    page_dir pd = new_page_dir();
    load_new_pd_status new_pd_status = load_new_pd((physical_address) &pd);
    assert_true(new_pd_status == LOAD_NEW_PD_SUCCESS);

    virtual_address some_virt_addr = 0x12345;
    get_phys_addr_resp phys_addr_trans = get_phys_addr(some_virt_addr);

    assert_true(phys_addr_trans.status == GET_PHYS_ADDR_PT_DNE);
}

static int get_phys_addr_pt_dne_test_teardown(void **state) {
    (void) state;

    clear_vm_config();

    return 0;
}

static void free_page_already_free_test(void **state) {
    (void) state;

    unsigned int num_pages_in_use_bef_free = get_num_pages_in_use();

    page_table_entry page_that_is_already_free = new_pte();
    free_page(&page_that_is_already_free);

    unsigned int num_pages_in_use_after_free = get_num_pages_in_use();

    assert_true(num_pages_in_use_bef_free == num_pages_in_use_after_free);
}

static void map_page_base_addr_pde_missing(void **state) {
    (void) state;

    page_dir pd = new_page_dir();
    load_new_pd_status load_blank_pd_status = load_new_pd((physical_address) &pd);
    assert_true(load_blank_pd_status == LOAD_NEW_PD_SUCCESS);

    physical_address new_frame_base_addr = 0x534;
    virtual_address meaningless_va = 0x12345;
    map_page_status failed_map_page_status = map_page_base_addr(new_frame_base_addr, meaningless_va);

    assert_true(failed_map_page_status == MAP_PAGE_PDE_OF_VA_MISSING);
}

static int map_page_base_addr_pde_missing_test_teardown(void **state) {
    (void) state;

    clear_vm_config();

    return 0;
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_teardown(alloc_page_success_test,
            alloc_page_test_teardown),
        cmocka_unit_test(free_page_test),
        cmocka_unit_test(get_page_table_entry_test),
        cmocka_unit_test(get_page_dir_entry_test),
        cmocka_unit_test_teardown(load_new_pd_test,
            load_new_pd_test_teardown),
        cmocka_unit_test(flush_tlb_entry_test),
        cmocka_unit_test_teardown(map_page_base_addr_success_test,
            map_page_base_addr_teardown),
        cmocka_unit_test(get_page_table_index_test),
        cmocka_unit_test(get_page_dir_index_test),
        cmocka_unit_test_teardown(get_curr_pdbr_test,
            get_curr_pdbr_test_teardown),
        cmocka_unit_test(new_page_dir_test),
        cmocka_unit_test_teardown(get_phys_addr_test,
            get_phys_addr_teardown),
        cmocka_unit_test_teardown(get_curr_pd_test,
            get_curr_pd_test_teardown),
        cmocka_unit_test(get_frame_offset_test),
        cmocka_unit_test(get_curr_pd_before_pd_init_test),
        cmocka_unit_test(new_virt_addr_test),
        cmocka_unit_test(clear_vm_init_test),
        cmocka_unit_test(get_pages_in_use_after_page_alloc_test),
        cmocka_unit_test_teardown(alloc_page_no_mem_avail_test,
            alloc_page_test_teardown),
        cmocka_unit_test_teardown(free_pd_success_test,
            free_pd_test_teardown),
        cmocka_unit_test(pd_4kb_aligned_test),
        cmocka_unit_test(page_table_4kb_aligned_test),
        cmocka_unit_test_teardown(map_page_failed_getting_pt_test,
            map_page_failed_getting_pt_test_teardown),
        cmocka_unit_test_teardown(get_phys_addr_pt_dne_test,
            get_phys_addr_pt_dne_test_teardown),
        cmocka_unit_test(free_pd_failed_getting_pt_test),
        cmocka_unit_test(free_page_already_free_test),
        cmocka_unit_test_teardown(map_page_base_addr_pde_missing,
            map_page_base_addr_pde_missing_test_teardown),
        cmocka_unit_test_teardown(init_vm_success_test,
            init_vm_test_teardown),
        cmocka_unit_test_teardown(init_vm_failed_mapping_all_pages_test,
            init_vm_test_teardown),
        cmocka_unit_test_teardown(init_vm_failed_loading_pd_test,
            init_vm_test_teardown),
        cmocka_unit_test_teardown(init_vm_kernel_space_test,
            init_vm_test_teardown),
        cmocka_unit_test_teardown(init_vm_user_space_test,
            init_vm_test_teardown),
        cmocka_unit_test(trans_to_kernel_phys_addr_test),
        cmocka_unit_test(trans_to_kernel_phys_addr_not_kernel_addr_test),
        cmocka_unit_test(alloc_page_failed_setting_pte_frame_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
