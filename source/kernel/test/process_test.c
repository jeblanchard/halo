#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "kernel/process.h"

static void test_create_proc(void **state) {
    (void) state;

    process new_proc = create_proc();

    (void) new_proc;

    assert_true(true);
}

static void test_destroy_proc(void **state) {
    (void) state;

    process new_proc = create_proc();

    destroy_proc(new_proc.id);

    assert_false(process_exists(new_proc.id));
}

static void test_stop_proc(void **state) {
    (void) state;

    process new_proc = create_proc();

    stop_proc(new_proc.id);

    assert_false(proc_is_running(new_proc.id));
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_proc),
        cmocka_unit_test(test_destroy_proc),
        cmocka_unit_test(test_stop_proc),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
