#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel/memory/virtual/pte.h"
#include "kernel/memory/virtual/pde/pde.h"

static void add_pte_attrib_test(void **state) {
    (void) state;

    page_table_entry fake_entry = 0;

    set_pte_attrib(&fake_entry, PTE_WRITABLE);

    page_table_entry correct_res = fake_entry | PTE_WRITABLE;

    assert_true(correct_res == fake_entry);
}

static void rm_pte_attrib_test(void **state) {
    (void) state;

    page_table_entry fake_entry = PTE_WRITABLE;

    rm_pte_attrib(&fake_entry, PTE_WRITABLE);

    page_table_entry correct_res = 0;

    assert_true(correct_res == fake_entry);
}

static void set_pte_frame_100000h_test(void **state) {
    (void) state;

    page_table_entry some_pte = 0;
    physical_address some_frame_base_addr = 0x100000;

    set_pte_frame_base_addr(&some_pte, some_frame_base_addr);

    physical_address actual_frame_base_addr = get_pte_frame_base_addr(&some_pte);
    assert_true(actual_frame_base_addr == some_frame_base_addr);
}

static void set_pte_frame_test(void **state) {
    (void) state;

    page_table_entry some_pte = 0;
    physical_address some_frame_base_addr = 3 * PAGE_SIZE_IN_BYTES;

    set_pte_frame_base_addr_status frame_set_success_status = \
        set_pte_frame_base_addr(&some_pte, some_frame_base_addr);
    assert_true(frame_set_success_status == SET_PTE_FRAME_BASE_ADDR_SUCCESS);

    physical_address actual_phys_addr = get_pte_frame_base_addr(&some_pte);

    assert_true(actual_phys_addr == some_frame_base_addr);
}

static void set_pte_frame_base_addr_frame_misaligned_test(void **state) {
    (void) state;

    page_table_entry some_pte = new_pte();
    physical_address misaligned_frame_base_addr = 3;

    set_pte_frame_base_addr_status misaligned_frame_status = \
        set_pte_frame_base_addr(&some_pte, misaligned_frame_base_addr);

    assert_true(misaligned_frame_status == FRAME_BASE_ADDR_NOT_ALIGNED);
}

static void get_pte_frame_test(void **state) {
    (void) state;

    page_table_entry some_entry = new_pte();
    physical_address some_frame_base_addr = 4 * PAGE_SIZE_IN_BYTES;

    set_pte_frame_base_addr_status frame_set_success_status = \
        set_pte_frame_base_addr(&some_entry, some_frame_base_addr);
    assert_true(frame_set_success_status == SET_PTE_FRAME_BASE_ADDR_SUCCESS);

    physical_address retrieved_frame = get_pte_frame_base_addr(&some_entry);

    assert_true(some_frame_base_addr == retrieved_frame);
}

static void pte_is_writeable_test(void **state) {
    (void) state;

    page_table_entry writeable_entry = PTE_WRITABLE;

    bool is_writeable = pte_is_writeable(&writeable_entry);

    assert_true(is_writeable);
}

static void pte_is_present_test(void **state) {
    (void) state;

    page_table_entry present_entry = PTE_PRESENT;

    bool is_present = page_is_present(&present_entry);

    assert_true(is_present);
}

static void is_attrib_set_test(void **state) {
    (void) state;

    page_table_entry entry = PTE_PRESENT | PTE_USER;

    bool is_present = is_pte_attrib_set(&entry, PTE_PRESENT);

    assert_true(is_present);
}

static void new_pte_test(void **state) {
    (void) state;

    page_table_entry pte = new_pte();

    assert_true(pte == (page_table_entry) 0);
}

static void frame_is_missing_test(void **state) {
    (void) state;

    page_table_entry pte = new_pte();

    assert_true(page_is_missing(&pte));
}

static void new_page_table_test(void **state) {
    (void) state;

    page_table new_pt = new_page_table();

    for (int i = 0; i < ENTRIES_PER_PAGE_DIR; i++) {
        assert_true(new_pt.entries[i] == new_pte());
    }
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(add_pte_attrib_test),
        cmocka_unit_test(rm_pte_attrib_test),
        cmocka_unit_test(set_pte_frame_test),
        cmocka_unit_test(set_pte_frame_base_addr_frame_misaligned_test),
        cmocka_unit_test(get_pte_frame_test),
        cmocka_unit_test(pte_is_writeable_test),
        cmocka_unit_test(pte_is_present_test),
        cmocka_unit_test(is_attrib_set_test),
        cmocka_unit_test(new_pte_test),
        cmocka_unit_test(frame_is_missing_test),
        cmocka_unit_test(new_page_table_test),
        cmocka_unit_test(set_pte_frame_100000h_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
