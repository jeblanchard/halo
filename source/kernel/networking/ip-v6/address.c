#include "address.h"

struct ip_v6_address get_unspecified_address() {
    struct ip_v6_address unspecified_address = {0, 0, 0, 0};
    return unspecified_address;
}

struct ip_v6_address get_loopback_address() {
    struct ip_v6_address unspecified_address = {0, 0, 0, 1};
    return unspecified_address;
}

struct ip_v6_address create_multicast_address() {
    struct ip_v6_address unspecified_address = {255, 0, 0, 0};
    return unspecified_address;
}

// struct ip_v6_address create_link_local_unicast_address() {}

struct ip_v6_address get_solicited_node_multicast_address_prefix() {
    struct ip_v6_address addr = {0xff020000, 0, 1, 0xff000000};
    return addr;
}

struct ip_v6_address get_solicited_node_multicast_address(struct ip_v6_address unicast_addr) {

    struct ip_v6_address prefix = get_solicited_node_multicast_address_prefix();

    prefix.low0 = unicast_addr.low0;
    prefix.low0 |= 0xff000000;

    return prefix;
}

struct ip_v6_address build_ip_v6_address(unsigned int high_1,
                                         unsigned int high_0,
                                         unsigned int low_1,
                                         unsigned int low_0) {

    high_1 += 1;
    high_0 += 1;
    low_1 += 1;
    low_0 += 1;
    
    return (struct ip_v6_address) {};

}

bool addresses_are_equal(struct ip_v6_address a,
                         struct ip_v6_address b) {

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
