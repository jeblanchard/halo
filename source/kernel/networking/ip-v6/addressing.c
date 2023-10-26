#include "addressing.h"

ip_v6_address get_unspecified_address() {
    ip_v6_address unspecified_address = {0, 0, 0,
                                         0, 0, 0,
                                         0, 0};

    return unspecified_address;
}

ip_v6_address get_loopback_address() {
    ip_v6_address loopback_address = {0, 0, 0,
                                      0, 0, 0,
                                      0, 1};

    return loopback_address;
}

ip_v6_address get_multicast_prefix() {
    ip_v6_address multicast_prefix = {0xff00, 0, 0, 0,
                                      0, 0, 0, 0};
    return multicast_prefix;
}

ip_v6_address get_link_local_unicast_prefix() {
    ip_v6_address link_local_unicast_prefix =
                                {0xfe80, 0, 0,
                                 0, 0, 0,
                                 0, 0};
                                 
    return link_local_unicast_prefix;
}

ip_v6_address get_solicited_node_multicast_address_prefix() {
    ip_v6_address prefix = {0xff02, 0,
                          0, 0, 0, 1,
                          0, 0};
    return prefix;
}

ip_v6_address get_solicited_node_multicast_address(ip_v6_address unicast_addr) {

    ip_v6_address prefix = get_solicited_node_multicast_address_prefix();

    prefix.order0 = unicast_addr.order0;
    prefix.order1 = unicast_addr.order1;

    prefix.order1 |= 0xff00;

    return prefix;
}

bool addresses_are_equal(ip_v6_address a,
                         ip_v6_address b) {

    if (a.order7 != b.order7) {
        return false;
    }

    if (a.order6 != b.order6) {
        return false;
    }

    if (a.order5 != b.order5) {
        return false;
    }

    if (a.order4 != b.order4) {
        return false;
    }

    if (a.order3 != b.order3) {
        return false;
    }

    if (a.order2 != b.order2) {
        return false;
    }

    if (a.order1 != b.order1) {
        return false;
    }

    if (a.order0 != b.order0) {
        return false;
    }

    return true;
}

static const ip_v6_address all_nodes_address_interface_local = {0xff01, 0, 0, 0, 0, 0, 0, 1};
static const ip_v6_address all_nodes_address_link_local = {0xff02, 0, 0, 0, 0, 0, 0, 1};

ip_v6_address get_all_nodes_address(all_nodes_scope scope) {
    if (scope == ALL_NODES_INTERFACE_LOCAL) {
        return all_nodes_address_interface_local;
    }

    return all_nodes_address_link_local;
}

static const ip_v6_address all_routers_address_interface_local = {0xff01, 0, 0, 0, 0, 0, 0, 2};
static const ip_v6_address all_routers_address_link_local = {0xff02, 0, 0, 0, 0, 0, 0, 2};
static const ip_v6_address all_routers_address_site_local = {0xff05, 0, 0, 0, 0, 0, 0, 2};

ip_v6_address get_all_routers_address(all_routers_scope scope) {
    if (scope == ALL_ROUTERS_INTERFACE_LOCAL) {
        return all_routers_address_interface_local;
    } else if (scope == ALL_ROUTERS_LINK_LOCAL) {
        return all_routers_address_link_local;
    }

    return all_routers_address_site_local;
}
