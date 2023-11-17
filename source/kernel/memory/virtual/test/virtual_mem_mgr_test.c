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
#include "kernel/memory/virtual/pde.h"

block_alloc_resp __wrap_alloc_block() {
    block_alloc_resp* resp = mock_ptr_type(block_alloc_resp*);
    return *resp;
}

static void alloc_page_test(void **state) {
    (void) state;

    page_table_entry entry = 0x12345678;

    block_alloc_resp non_zero_buff_addr_resp = {
        status: BLOCK_ALLOC_SUCCESS,
        buffer_size: 100,
        buffer: (void*) 0xfff
    };

    will_return(__wrap_alloc_block, (uintptr_t) &non_zero_buff_addr_resp);

    alloc_page_resp resp = alloc_page(&entry);

    assert_true(resp == SUCCESS);
    assert_true(pte_is_present(&entry));

    assert_true(get_pte_frame(&entry) == (physical_address) non_zero_buff_addr_resp.buffer);
}

bool correct_block_addr_was_freed = false;
page_table_entry entry_of_page_to_free = 0x12345678 | PDE_PRESENT | PDE_WRITABLE;

void __wrap_free_block(void* block_address) {
    physical_address correct_addr_to_free = \
        get_pte_frame(&entry_of_page_to_free);

    if (correct_addr_to_free == (physical_address) block_address) {
        correct_block_addr_was_freed = true;
    }
}

static void free_page_test(void **state) {
    (void) state;

    free_page(&entry_of_page_to_free);

    assert_false(pte_is_present(&entry_of_page_to_free));
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

    page_table_entry fake_page_table[3] = {
        (page_table_entry) 0x12345678,
        (page_table_entry) 0x91011121,
        (page_table_entry) 0x13141516
    };

    virtual_address entry_2_virt_addr = 0x1000;

    page_table_entry* actual_pte = \
        get_page_table_entry(fake_page_table, entry_2_virt_addr);
 
    assert_ptr_equal(actual_pte, &fake_page_table[1]);
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

    (void) state;

    page_dir_entry fake_page_dir_table[3] = {
        (page_dir_entry) 0x12345678,
        (page_dir_entry) 0x91011121,
        (page_dir_entry) 0x13141516
    };

    virtual_address entry_2_virt_addr = 0x400000;

    page_dir_entry* actual_pde = \
        get_page_dir_entry(fake_page_dir_table, entry_2_virt_addr);
 
    assert_ptr_equal(actual_pde, &fake_page_dir_table[1]);
}

static void switch_page_dir_test(void **state) {
    (void) state;

    // page_dir pd = // somehow get the PD
    // page_table_entry pt = switch_page_dir(p_table, v_addr);

    // assert the new curr page dir is the correct
    // one
    assert_true(false);
}

static void flush_tlb_entry_test(void **state) {
    (void) state;

    // virtual_addr v_addr = // somehow get the virtual addr
    // flush_tlb_entry(v_addr);

    // assert that the tlb entry has been flushed properly
    assert_true(false);
}

static void map_page_test(void **state) {
    (void) state;

    // set up the page directory so it is valid

    // virtual_addr v_addr = // somehow get the virtual addr
    // physical_addr phys_addr = // somehow get the phys addr
    // map_page(phys_addr, v_addr);

    // assert that the address has been mapped
    // properly
    assert_true(false);
}

static void add_pte_attrib_test(void **state) {
    (void) state;

    // add_pte_attrib();

    // check that everything was initialized
    // properly
    assert_true(false);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(alloc_page_test),
        cmocka_unit_test(free_page_test),
        cmocka_unit_test(get_page_table_entry_test),
        cmocka_unit_test(get_page_dir_entry_test),
        cmocka_unit_test(switch_page_dir_test),
        cmocka_unit_test(flush_tlb_entry_test),
        cmocka_unit_test(add_pte_attrib_test),
        cmocka_unit_test(map_page_test),
        cmocka_unit_test(get_page_table_index_test),
        cmocka_unit_test(get_page_dir_index_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
