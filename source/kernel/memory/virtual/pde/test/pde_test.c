#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel/memory/virtual/pde/pde.h"
#include "kernel/memory/virtual/pte.h"

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

    page_dir_entry pde = new_pde();
    page_table pt = new_page_table();

    set_pt_base_addr(&pde, (physical_address) &pt);

    pt_base_addr_resp actual_pt_base_addr_resp = get_page_table_base_addr(&pde);
    assert_true(actual_pt_base_addr_resp.status == PT_BASE_ADDR_SUCCESS);

    assert_true((physical_address) &pt == actual_pt_base_addr_resp.pt_base_addr);
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

static void pde_is_kernel_space_test(void **state) {
    (void) state;

    page_dir_entry kernel_entry = 0;
    add_pde_attrib(&kernel_entry, PDE_USER);
    assert_true(is_pde_attrib_set(&kernel_entry, PDE_USER));

    rm_pde_attrib(&kernel_entry, PDE_USER);

    assert_true(is_kernel_pde(&kernel_entry));
}

static void get_page_table_base_addr_test(void **state) {
    (void) state;

    page_dir pd = new_page_dir();
    page_table pt = new_page_table();

    unsigned int pd_index = 0;
    set_pt_base_addr(&pd.entries[pd_index], (physical_address) &pt);

    pt_base_addr_resp actual_pt_base_addr_resp = \
        get_page_table_base_addr(&pd.entries[pd_index]);
    assert_true(actual_pt_base_addr_resp.status == PT_BASE_ADDR_SUCCESS);

    assert_true(actual_pt_base_addr_resp.pt_base_addr == \
        (physical_address) &pt);
}

static void get_pt_base_addr_pt_dne_test(void **state) {
    (void) state;

    page_dir_entry pde = new_pde();

    pt_base_addr_resp pt_resp = get_page_table_base_addr(&pde);
    
    assert_true(pt_resp.status == PT_DNE);
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
        cmocka_unit_test(is_attrib_set_test),
        cmocka_unit_test(new_blank_pde_test),
        cmocka_unit_test(pde_is_kernel_space_test),
        cmocka_unit_test(get_page_table_base_addr_test),
        cmocka_unit_test(get_pt_base_addr_pt_dne_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
