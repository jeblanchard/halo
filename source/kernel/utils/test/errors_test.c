#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"

#include "../errors.h"

static void test_simple_error(void **state) {
    (void) state;

    return assert_true(false);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_simple_error),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
