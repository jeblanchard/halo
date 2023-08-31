#pragma pack(push, 1)
struct ip_v6_address {
    unsigned int high_1;
    unsigned int high_0;
    unsigned int low_1;
    unsigned int low_0;
};
#pragma pack(pop)

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

struct ip_v6_address get_all_routers_multicast_address() {
    struct ip_v6_address unspecified_address = {255, 0, 0, 0};
    return unspecified_address;
}

#define LINK_LOCAL_ADDRESS_PREFIX 0xfe80

struct ip_v6_address get_link_local_address() {
    struct mac_address host_mac_addr = get_host_mac_address();
    struct ip_v6_address addr = {LINK_LOCAL_ADDRESS_PREFIX,
                                 host_mac_addr.low,
                                 host_mac_addr.mid,
                                 host_mac_addr.high}

    return addr;
}

struct ip_v6_address create_link_local_unicast_address() {}

struct ip_v6_address get_solicited_node_multicast_address_prefix() {
    struct ip_v6_address addr = {0xff020000, 0, 1, 0xff000000};
    return addr;
}

struct ip_v6_address get_solicited_node_multicast_address(struct ip_v6_address unicast_addr) {

    struct ip_v6_address prefix = get_solicited_node_multicast_address();

    prefix.low_0 = unicast_addr.low_0;
    prefix.low_0 |= 0xff000000

    return prefix;
}
