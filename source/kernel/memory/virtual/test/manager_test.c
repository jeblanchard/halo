#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel/memory/virtual/pte.h"
#include "kernel/memory/virtual/manager.h"



static void alloc_page_test(void **state) {
    (void) state;

    // page_table_entry entry = 

    // alloc_page_resp resp = alloc_page(page_table_entry);

    // assert that the frame address is correct and
    // that the present bit is set in the PTE
    assert_true(false);
}

static void free_page_test(void **state) {
    (void) state;

    // page_table_entry entry = // somehow grab the PTE
    // alloc_page_resp resp = free_page(page_table_entry);

    // assert that the physical memory is now free
    // and that the present bit of the PTE is
    // set to NOT PRESENT
    assert_true(false);
}

static void get_page_table_entry_test(void **state) {
    (void) state;

    // virtual_addr v_addr = // somehow get the virtual addr
    // page_table p_table = // somehow get the page table
    // page_table_entry pt = get_page(p_table, v_addr);

    // assert that the page table is the correct one 
    assert_true(false);
}

static void get_page_dir_entry_test(void **state) {
    (void) state;

    // virtual_addr v_addr = // somehow get the virtual addr
    // page_table p_table = // somehow get the page table
    // page_dir_entry pt = get_page_dir_entry(p_table, v_addr);

    // assert that the PDE is the correct one 
    assert_true(false);
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
        cmocka_unit_test(map_page_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
