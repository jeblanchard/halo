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

    entry = new_pte();

    physical_address buffer_base_addr = 0xfff;
    alloc_block_resp block_resp = {
        status: ALLOC_BLOCK_SUCCESS,
        buffer_size: BYTES_PER_MEMORY_UNIT,
        buffer: buffer_base_addr
    };

    will_return(__wrap_alloc_block, (uintptr_t) &block_resp);

    alloc_page_status status = alloc_page(&entry);

    assert_true(status == ALLOC_PAGE_SUCCESS);
    assert_true(page_is_present(&entry));
    assert_true(get_pte_frame_base(&entry) == buffer_base_addr);
}

static void alloc_page_no_mem_avail_test(void **state) {
    (void) state;

    entry = new_pte();

    alloc_block_resp no_mem_alloc_block_resp = {
        status: NO_FREE_BLOCKS,
        buffer_size: 0,
        buffer: NULL
    };
    
    will_return(__wrap_alloc_block, (uintptr_t) &no_mem_alloc_block_resp);

    alloc_page_status status = alloc_page(&entry);

    assert_true(status == ALLOC_PAGE_NO_MEM_AVAIL);
    assert_true(page_is_present(&entry) == false);
}

static void alloc_page_gen_failure_test(void **state) {
    (void) state;

    entry = new_pte();

    alloc_block_resp could_not_get_free_frame_resp = {
        status: COULD_NOT_GET_FREE_FRAME,
        buffer_size: 0,
        buffer: NULL
    };

    will_return(__wrap_alloc_block, (uintptr_t) &could_not_get_free_frame_resp);

    alloc_page_status status = alloc_page(&entry);

    assert_true(status == ALLOC_PAGE_GEN_FAILURE);
    assert_true(page_is_present(&entry) == false);
}

static void get_pages_in_use_after_page_alloc_test(void **state) {
    (void) state;

    page_table_entry entry = new_pte();

    alloc_block_resp non_zero_buff_addr_resp = {
        status: ALLOC_BLOCK_SUCCESS,
        buffer_size: BYTES_PER_MEMORY_BLOCK,
        buffer: NULL
    };

    will_return(__wrap_alloc_block, (uintptr_t) &non_zero_buff_addr_resp);

    unsigned int og_pages_in_use = get_pages_in_use();

    alloc_page_status status = alloc_page(&entry);
    assert_true(status == ALLOC_PAGE_SUCCESS);

    unsigned int new_pages_in_use = get_pages_in_use();

    assert_true(new_pages_in_use == og_pages_in_use + 1);
}

bool correct_block_addr_was_freed = false;
page_table_entry entry_of_page_to_free;
physical_address fake_frame_base_addr = 0x12345;

void __wrap_free_block(physical_address block_base_addr) {
    if (fake_frame_base_addr == block_base_addr) {
        correct_block_addr_was_freed = true;
    }
}

static void free_page_test(void **state) {
    (void) state;

    entry_of_page_to_free = new_pte();
    add_pte_attrib(&entry_of_page_to_free, PTE_WRITABLE);
    add_pte_attrib(&entry_of_page_to_free, PTE_PRESENT);
    set_pte_frame_base_addr(&entry_of_page_to_free,
        (physical_address) fake_frame_base_addr);
        
    free_page(&entry_of_page_to_free);

    assert_false(page_is_present(&entry_of_page_to_free));
    assert_true(correct_block_addr_was_freed);
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

physical_address __wrap_get_pt_base_addr(page_dir_entry* pde) {
    (void) pde;
    return mock();
}

static void map_page_base_addr_test(void **state) {
    (void) state;

    unsigned int pd_index = 1;
    unsigned int pt_index = 4;
    unsigned int page_offset = 300;

    new_virt_addr_resp new_va_resp = new_virt_addr(pd_index, pt_index, page_offset);
    assert_true(new_va_resp.status == NEW_VIRT_ADDR_SUCCESS);

    page_dir pd = new_page_dir();
    page_table pt = new_page_table();
    physical_address frame_base_addr = 0x1234;
    
    set_pt_base_addr(&pd.entries[pd_index], (physical_address) &pt);
    add_pde_attrib(&pd.entries[pd_index], PDE_PRESENT);

    set_pte_frame_base_addr(&pt.entries[pt_index], (physical_address) frame_base_addr);
    add_pte_attrib(&pt.entries[pt_index], PTE_PRESENT);

    will_return(__wrap_get_pt_base_addr, (physical_address) &pt);

    load_new_pd_status new_pd_status = load_new_pd((physical_address) &pd);
    assert_true(new_pd_status == LOAD_NEW_PD_SUCCESS);

    will_return(__wrap_get_pt_base_addr, (physical_address) &pt);

    get_phys_addr_resp og_phys_addr_resp = get_phys_addr(new_va_resp.virt_addr);
    assert_true(og_phys_addr_resp.status == GET_PHYS_ADDR_SUCCESS);

    physical_address new_frame_base_addr = 0x534;

    map_page_status mp_status = map_page_base_addr(new_frame_base_addr, new_va_resp.virt_addr);
    assert_true(mp_status == MP_SUCCESS);

    will_return(__wrap_get_pt_base_addr, (physical_address) &pt);

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

    page_dir fake_pd = new_page_dir();
    page_table fake_pt = new_page_table();
    physical_address fake_frame_base_addr = 0x12345;

    page_table_entry* correct_pte = &fake_pt.entries[pt_index];
    set_pte_frame_base_addr(correct_pte, fake_frame_base_addr);

    load_new_pd((physical_address) &fake_pd);

    will_return(__wrap_get_pt_base_addr, (physical_address) &fake_pt);

    get_phys_addr_resp phys_addr_resp = get_phys_addr(new_va_resp.virt_addr);
    assert_true(phys_addr_resp.status == GET_PHYS_ADDR_SUCCESS);

    physical_address correct_phys_addr = fake_frame_base_addr + page_offset;
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

static void new_page_table_test(void **state) {
    (void) state;

    page_table new_pt = new_page_table();

    for (int i = 0; i < ENTRIES_PER_PAGE_DIR; i++) {
        assert_true(new_pt.entries[i] == new_pte());
    }
}

static void get_frame_offset_test(void **state) {
    (void) state;

    unsigned int frame_offset = 23;

    new_virt_addr_resp new_va_resp = new_virt_addr(0, 0, frame_offset);
    assert_true(new_va_resp.status == NEW_VIRT_ADDR_SUCCESS);

    assert_true(get_frame_offset(new_va_resp.virt_addr) == frame_offset);
}

static int init_vm_test_teardown(void **state) {
    (void) state;

    clear_vm_config();

    return 0;
}

page_dir* curr_pd;

static int init_vm_test_setup(void **state) {
    (void) state;

    init_vm_status status = init_virtual_mem();
    assert_true(status == INIT_VM_SUCCESS);

    get_curr_pd_resp curr_pd_resp = get_curr_pd();
    assert_true(curr_pd_resp.status == GET_CURR_PD_SUCCESS);

    curr_pd = curr_pd_resp.curr_pd;

    return 0;
}

static void init_vm_io_addr_range_test(void **state) {
    (void) state;

    #define ONE_KB 1024
    for (virtual_address io_addr = IO_BASE_ADDR; io_addr <= IO_MAX_ADDR; io_addr += ONE_KB) {
        page_dir_entry* pde = get_page_dir_entry(curr_pd, io_addr);

        assert_true(pde_is_present(pde));
        assert_true(is_kernel_pde(pde));
    }
}

static void init_vm_kernel_addr_range_test(void **state) {
    (void) state;

    for (virtual_address kernel_addr = KERNEL_SPACE_BASE_VIRT_ADDR;
         kernel_addr <= KERNEL_SPACE_MAX_ADDR;
         kernel_addr += ONE_KB) {

        page_dir_entry* pde = get_page_dir_entry(curr_pd, kernel_addr);

        assert_true(pde_is_present(pde));
        assert_true(is_kernel_pde(pde));
    }
}

static void init_vm_user_addr_range_test(void **state) {
    (void) state;

    for (virtual_address user_addr = USER_SPACE_BASE_ADDR;
         user_addr <= USER_SPACE_MAX_ADDR;
         user_addr += ONE_KB) {
            
        page_dir_entry* pde = get_page_dir_entry(curr_pd, user_addr);

        assert_true(pde_is_present(pde));
        assert_true(pde_is_user(pde));
    }
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
        cmocka_unit_test_teardown(map_page_base_addr_test,
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
        cmocka_unit_test(new_page_table_test),
        cmocka_unit_test_setup_teardown(init_vm_io_addr_range_test,
            init_vm_test_setup,
            init_vm_test_teardown),
        cmocka_unit_test_setup_teardown(init_vm_kernel_addr_range_test,
            init_vm_test_setup,
            init_vm_test_teardown),
        cmocka_unit_test_setup_teardown(init_vm_user_addr_range_test,
            init_vm_test_setup,
            init_vm_test_teardown),
        cmocka_unit_test(get_pages_in_use_after_page_alloc_test),
        cmocka_unit_test_teardown(alloc_page_no_mem_avail_test,
            alloc_page_test_teardown),
        cmocka_unit_test_teardown(alloc_page_gen_failure_test,
            alloc_page_test_teardown)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
