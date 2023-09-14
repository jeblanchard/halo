#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>

static void basic_test(void **state) {
    (void) state;
    assert_true(7);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(basic_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
