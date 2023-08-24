#pragma pack(push, 1)
struct ip_v6_address {
    unsigned int low_0;
    unsigned int low_1;
    unsigned int high_0;
    unsigned int high_1;
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

struct ip_v6_address create_link_local_unicast_address() {}
