#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"

static void test_create_proc(void **state) {
    (void) state;

    process new_proc = create_proc();

    (void) new_proc;

    assert_true(true);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_proc),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}