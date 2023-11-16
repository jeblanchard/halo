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

    add_pde_attrib(&fake_entry, WRITABLE);

    page_dir_entry correct_res = fake_entry | WRITABLE;

    assert_true(correct_res == fake_entry);
}

static void rm_pde_attrib_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = WRITABLE;

    rm_pde_attrib(&fake_entry, WRITABLE);

    page_dir_entry correct_res = 0;

    assert_true(correct_res == fake_entry);
}

static void set_pde_frame_test(void **state) {
    (void) state;

    physical_address frame = 0x1234;
    page_dir_entry fake_entry = 0;

    set_pde_frame(&fake_entry, frame);

    page_dir_entry correct_res = frame << 12;

    assert_true(correct_res == fake_entry);
}

static void pde_is_present_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = PRESENT;

    bool is_present = pde_is_present(&fake_entry);

    assert_true(is_present);
}

static void pde_is_user_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = USER;

    bool is_user = pde_is_user(&fake_entry);

    assert_true(is_user);
}

static void pde_is_4mb_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = SIZE_4MB;

    bool is_4mb = pde_is_4mb(&fake_entry);

    assert_true(is_4mb);
}

static void pde_is_writeable_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = WRITABLE;

    bool is_writeable = pde_is_writeable(&fake_entry);

    assert_true(is_writeable);
}

static void get_pde_frame_test(void **state) {
    (void) state;

    physical_address frame = 0x1234;
    page_dir_entry fake_entry = frame << 12;

    physical_address retrieved_frame = get_pde_frame(&fake_entry);

    assert_true(retrieved_frame == frame);
}

static void enable_global_test(void **state) {
    (void) state;

    page_dir_entry fake_entry = 0;

    enable_global(&fake_entry);

    page_dir_entry correct_entry = CPU_GLOBAL;

    assert_true(fake_entry == correct_entry);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(add_pde_attrib_test),
        cmocka_unit_test(rm_pde_attrib_test),
        cmocka_unit_test(set_pde_frame_test),
        cmocka_unit_test(pde_is_present_test),
        cmocka_unit_test(pde_is_user_test),
        cmocka_unit_test(pde_is_4mb_test),
        cmocka_unit_test(pde_is_writeable_test),
        cmocka_unit_test(get_pde_frame_test),
        cmocka_unit_test(enable_global_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
