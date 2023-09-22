#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"

#include "icmp_v6.h"

static void test_build_icmp_prefix(void **state) {
    (void) state;

    unsigned char type = 1;
    unsigned char code = 2;

    struct icmp_message_prefix correct_prefix = {type, code};
    struct icmp_message_prefix actual_prefix = \ 
        build_icmp_message_prefix(type, code);

    assert_int_equal(actual_prefix.type, correct_prefix.type);
    assert_int_equal(actual_prefix.code, correct_prefix.code);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_build_icmp_prefix)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
