#include "../../utils/standard.h"

#pragma pack(push, 1)
struct ip_v6_address {
    unsigned int high1;
    unsigned int high0;
    unsigned int low1;
    unsigned int low0;
};
#pragma pack(pop)

bool addresses_are_equal(struct ip_v6_address a,
                         struct ip_v6_address b);

struct ip_v6_address get_unspecified_address();

struct ip_v6_address get_loopback_address();

struct ip_v6_address create_multicast_address();

struct ip_v6_address create_link_local_unicast_address();

struct ip_v6_address get_solicited_node_multicast_address_prefix();

struct ip_v6_address get_solicited_node_multicast_adddress();

struct ip_v6_address get_link_layer_address();

struct ip_v6_address build_ip_v6_address(unsigned int high_1,
                                         unsigned int high_0,
                                         unsigned int low_1,
                                         unsigned int low_0);
