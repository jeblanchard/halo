#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel/memory/virtual/pte.h"

static void add_pte_attrib_test(void **state) {
    (void) state;

    page_table_entry fake_entry = 0;

    add_pte_attrib(&fake_entry, WRITABLE);

    page_table_entry correct_res = fake_entry | WRITABLE;

    assert_true(correct_res == fake_entry);
}

static void rm_pte_attrib_test(void **state) {
    (void) state;

    page_table_entry fake_entry = WRITABLE;

    rm_pte_attrib(&fake_entry, WRITABLE);

    page_table_entry correct_res = 0;

    assert_true(correct_res == fake_entry);
}

static void set_pte_frame_test(void **state) {
    (void) state;

    page_table_entry fake_entry = 0;
    physical_address phys_addr = 0x1234;

    set_pte_frame(&fake_entry, phys_addr);

    page_table_entry correct_res = phys_addr << 12;

    assert_true(correct_res == fake_entry);
}

static void get_pte_frame_test(void **state) {
    (void) state;

    physical_address correct_frame = 0x1234;
    page_table_entry fake_entry = correct_frame << 12;

    physical_address retrieved_frame = get_pte_frame(&fake_entry);

    assert_true(correct_frame == retrieved_frame);
}

static void pte_is_writeable_test(void **state) {
    (void) state;

    page_table_entry writeable_entry = WRITABLE;

    bool is_writeable = pte_is_writeable(&writeable_entry);

    assert_true(is_writeable);
}

static void pte_is_present_test(void **state) {
    (void) state;

    page_table_entry present_entry = PRESENT;

    bool is_present = pte_is_present(&present_entry);

    assert_true(is_present);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(add_pte_attrib_test),
        cmocka_unit_test(rm_pte_attrib_test),
        cmocka_unit_test(set_pte_frame_test),
        cmocka_unit_test(get_pte_frame_test),
        cmocka_unit_test(pte_is_writeable_test),
        cmocka_unit_test(pte_is_present_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
