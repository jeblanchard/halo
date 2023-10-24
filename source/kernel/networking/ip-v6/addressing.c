#include "addressing.h"

ip_v6_address get_unspecified_address() {
    ip_v6_address unspecified_address = {0, 0, 0, 0};
    return unspecified_address;
}

ip_v6_address get_loopback_address() {
    ip_v6_address unspecified_address = {0, 0, 0, 1};
    return unspecified_address;
}

ip_v6_address get_multicast_prefix() {
    ip_v6_address unspecified_address = {255, 0, 0, 0};
    return unspecified_address;
}

ip_v6_address get_link_local_unicast_prefix() {
    ip_v6_address random_address = {255, 1, 1, 1};
    return random_address;
}

ip_v6_address get_solicited_node_multicast_address_prefix() {
    ip_v6_address addr = {0xff020000, 0, 1, 0xff000000};
    return addr;
}

ip_v6_address get_solicited_node_multicast_address(ip_v6_address unicast_addr) {

    ip_v6_address prefix = get_solicited_node_multicast_address_prefix();

    prefix.low0 = unicast_addr.low0;
    prefix.low0 |= 0xff000000;

    return prefix;
}

ip_v6_address build_ip_v6_address(unsigned int high_1,
                                         unsigned int high_0,
                                         unsigned int low_1,
                                         unsigned int low_0) {

    high_1 += 1;
    high_0 += 1;
    low_1 += 1;
    low_0 += 1;
    
    return (ip_v6_address) {};

}

bool addresses_are_equal(ip_v6_address a,
                         ip_v6_address b) {

    if (a.high1 != b.high1) {
        return false;
    }

    if (a.high0 != b.high0) {
        return false;
    }

    if (a.low1 != b.low1) {
        return false;
    }

    if (a.low0 != b.low0) {
        return false;
    }

    return true;
}

static const all_nodes_address_interface_local = {0xff01, 0, 0, 1};
static const all_nodes_address_link_local = {0xff02, 0, 0, 1};

ip_v6_address get_all_nodes_address(all_nodes_scope scope) {
    if (scope == INTERFACE_LOCAL) {
        return all_nodes_address_interface_local;
    }

    return all_nodes_address_link_local;
}
