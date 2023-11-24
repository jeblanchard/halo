#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel/memory/virtual/pde.h"

static void add_pde_attrib_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = 0;

    add_pde_attrib(&fake_entry, PDE_WRITABLE);

    page_dir_entry correct_res = fake_entry | PDE_WRITABLE;

    assert_true(correct_res == fake_entry);
}

static void rm_pde_attrib_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = PDE_WRITABLE;

    rm_pde_attrib(&fake_entry, PDE_WRITABLE);

    page_dir_entry correct_res = 0;

    assert_true(correct_res == fake_entry);
}

static void set_pt_addr_test(void **state) {
    (void) state;

    physical_address pt_addr = 0x1234;
    page_dir_entry fake_entry = 0;

    set_pt_base_addr(&fake_entry, pt_addr);

    page_dir_entry correct_res = pt_addr << 12;

    assert_true(correct_res == fake_entry);
}

static void pde_is_present_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = PDE_PRESENT;

    bool is_present = pde_is_present(&fake_entry);

    assert_true(is_present);
}

static void pde_is_user_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = PDE_USER;

    bool is_user = pde_is_user(&fake_entry);

    assert_true(is_user);
}

static void pde_is_4mb_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = PDE_SIZE_4MB;

    bool is_4mb = pde_is_4mb(&fake_entry);

    assert_true(is_4mb);
}

static void pde_is_writeable_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = PDE_WRITABLE;

    bool is_writeable = pde_is_writeable(&fake_entry);

    assert_true(is_writeable);
}

static void get_page_table_base_addr_test(void **state) {
    (void) state;

    physical_address pt_addr = 0x1234;
    page_dir_entry fake_entry = pt_addr << 12;

    assert_true(get_page_table_base_addr(&fake_entry) == pt_addr);
}

static void enable_global_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = 0;

    enable_global(&fake_entry);

    page_dir_entry correct_entry = PDE_CPU_GLOBAL;

    assert_true(fake_entry == correct_entry);
}

static void is_attrib_set_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = PDE_WRITABLE | PDE_USER;

    bool is_writable = is_pde_attrib_set(&fake_entry, PDE_WRITABLE);

    assert_true(is_writable);
}

static void new_blank_pde_test(void **state) {
    (void) state;

    page_dir_entry blank_entry = 0;

    assert_true(blank_entry == new_pde());
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(add_pde_attrib_test),
        cmocka_unit_test(rm_pde_attrib_test),
        cmocka_unit_test(set_pt_addr_test),
        cmocka_unit_test(pde_is_present_test),
        cmocka_unit_test(pde_is_user_test),
        cmocka_unit_test(pde_is_4mb_test),
        cmocka_unit_test(pde_is_writeable_test),
        cmocka_unit_test(get_page_table_base_addr_test),
        cmocka_unit_test(enable_global_test),
        cmocka_unit_test(is_attrib_set_test),
        cmocka_unit_test(new_blank_pde_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
