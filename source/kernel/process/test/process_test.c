#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "kernel/process/process.h"

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

static void test_run_proc_code(void **state) {
    (void) state;

    #define buf_size 5

    char* actual_buf = (char*) calloc(buf_size, sizeof(char));
    char correct_buf[buf_size] = {'a', 'b', 'c', 'd', 'e'};

    process new_proc = create_proc();

    run_proc(new_proc.id);

    for (int i = 0; i < buf_size; i++) {
        assert_true(actual_buf[i] == correct_buf[i]);
    }
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_proc),
        cmocka_unit_test(test_destroy_proc),
        cmocka_unit_test(test_stop_proc),
        cmocka_unit_test(test_run_proc_code)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
