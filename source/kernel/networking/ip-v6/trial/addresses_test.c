#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>

#include "addresses.h"
#include "../../../utils/standard.h"


static void test_address_are_equal(void **state) {
    (void) state;

    struct ip_v6_address a = {1, 2, 3, 4};
    struct ip_v6_address b = {1, 2, 3, 4};

    assert_true(addresses_are_equal(a, b));
}

static void test_get_unspecified_address(void **state) {
    (void) state;

    struct ip_v6_address unspecified_address = {0, 0, 0, 0};
    
    bool is_correct = addresses_are_equal(get_unspecified_address(),
                                          unspecified_address);

    assert_true(is_correct);
}

static void test_get_loopback_address(void **state) {
    (void) state;

    struct ip_v6_address loopback_address = {0, 0, 0, 0};
    
    bool is_correct = addresses_are_equal(get_loopback_address(),
                                          loopback_address);

    assert_true(is_correct);
}

static void test_create_multicast_address(void **state) {
    (void) state;

    struct ip_v6_address multicast_address = {0, 0, 0, 0};
    
    bool is_correct = addresses_are_equal(create_multicast_address(),
                                          multicast_address);

    assert_true(is_correct);
}

static void test_get_solicited_node_multicast_address_prefix(void **state) {
    (void) state;

    struct ip_v6_address solicited_node_multicast_address_prefix = {0, 0, 0, 0};
    
    bool is_correct = addresses_are_equal(get_solicited_node_multicast_address_prefix(),
                                          solicited_node_multicast_address_prefix);

    assert_true(is_correct);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_address_are_equal),
        cmocka_unit_test(test_get_unspecified_address),
        cmocka_unit_test(test_get_loopback_address),
        cmocka_unit_test(test_create_multicast_address),
        cmocka_unit_test(test_get_solicited_node_multicast_address_prefix),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
