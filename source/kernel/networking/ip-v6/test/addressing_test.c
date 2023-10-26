#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdbool.h>

#include "kernel/networking/ip-v6/addressing.h"


static void test_addresses_are_equal(void **state) {
    (void) state;

    ip_v6_address a = {1, 2, 3, 4, 5, 6, 7, 8};
    ip_v6_address b = {1, 2, 3, 4, 5, 6, 7, 8};

    assert_true(addresses_are_equal(a, b));
}

static void test_get_unspecified_address(void **state) {
    (void) state;

    ip_v6_address unspecified_address = {0, 0, 0, 0, 0, 0, 0, 0};
    
    bool is_correct = addresses_are_equal(get_unspecified_address(),
                                          unspecified_address);

    assert_true(is_correct);
}

static void test_get_loopback_address(void **state) {
    (void) state;

    ip_v6_address loopback_address = {0, 0, 0, 0, 0, 0, 0, 1};
    
    bool is_correct = addresses_are_equal(get_loopback_address(),
                                          loopback_address);

    assert_true(is_correct);
}

static void test_get_multicast_prefix(void **state) {
    (void) state;

    ip_v6_address multicast_prefix = {0xff00, 0, 0, 0, 0, 0, 0, 0};
    
    bool is_correct = addresses_are_equal(get_multicast_prefix(),
                                          multicast_prefix);

    assert_true(is_correct);
}

static void test_get_link_local_unicast_prefix(void **state) {
    (void) state;

    ip_v6_address link_local_unicast_prefix = {0xfe80, 0, 0, 0, 0, 0, 0, 0};
    
    bool is_correct = addresses_are_equal(get_link_local_unicast_prefix(),
                                          link_local_unicast_prefix);

    assert_true(is_correct);
}

static void test_get_all_nodes_address(void **state) {
    (void) state;

    ip_v6_address all_nodes_address_interface_local = {0xff01, 0, 0, 0, 0, 0, 0, 1};
    ip_v6_address all_nodes_address_link_local = {0xff02, 0, 0, 0, 0, 0, 0, 1};
    
    bool interface_local_is_correct = addresses_are_equal(get_all_nodes_address(ALL_NODES_INTERFACE_LOCAL),
                                          all_nodes_address_interface_local);

    bool link_local_is_correct = addresses_are_equal(get_all_nodes_address(ALL_NODES_LINK_LOCAL),
                                          all_nodes_address_link_local);

    assert_true(interface_local_is_correct);
    assert_true(link_local_is_correct);
}

static void test_get_all_routers_address(void **state) {
    (void) state;

    ip_v6_address all_routers_address_interface_local = {0xff01, 0, 0, 0, 0, 0, 0, 2};
    ip_v6_address all_routers_address_link_local = {0xff02, 0, 0, 0, 0, 0, 0, 2};
    ip_v6_address all_routers_address_site_local = {0xff05, 0, 0, 0, 0, 0, 0, 2};

    
    bool interface_local_is_correct = addresses_are_equal(get_all_routers_address(ALL_ROUTERS_INTERFACE_LOCAL),
                                          all_routers_address_interface_local);

    bool link_local_is_correct = addresses_are_equal(get_all_routers_address(ALL_ROUTERS_LINK_LOCAL),
                                          all_routers_address_link_local);

    bool site_local_is_correct = addresses_are_equal(get_all_routers_address(ALL_ROUTERS_SITE_LOCAL),
                                          all_routers_address_site_local);

    assert_true(interface_local_is_correct);
    assert_true(link_local_is_correct);
    assert_true(site_local_is_correct);
}

static void test_get_solicited_node_multicast_address_prefix(void **state) {
    (void) state;

    ip_v6_address solicited_node_multicast_address_prefix = {0xff02, 0, 0, 0, 0, 1, 0, 0};
    
    bool is_correct = addresses_are_equal(get_solicited_node_multicast_address_prefix(),
                                          solicited_node_multicast_address_prefix);

    assert_true(is_correct);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_addresses_are_equal),
        cmocka_unit_test(test_get_unspecified_address),
        cmocka_unit_test(test_get_loopback_address),
        cmocka_unit_test(test_get_multicast_prefix),
        cmocka_unit_test(test_get_solicited_node_multicast_address_prefix),
        cmocka_unit_test(test_get_all_routers_address),
        cmocka_unit_test(test_get_all_nodes_address),
        cmocka_unit_test(test_get_link_local_unicast_prefix)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
