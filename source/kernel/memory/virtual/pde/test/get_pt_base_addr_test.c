#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel/memory/virtual/pde/get_pt_base_addr.h"

static void get_page_table_base_addr_test(void **state) {
    (void) state;

    physical_address pt_addr = 0x1234;
    page_dir_entry fake_entry = pt_addr << 12;

    assert_true((physical_address) get_pt_base_addr(&fake_entry) == pt_addr);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(get_page_table_base_addr_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
