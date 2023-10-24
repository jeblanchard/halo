#pragma once

#include "kernel/utils/standard.h"

#pragma pack(push, 1)
typedef struct ip_v6_address {
    unsigned short order7;
    unsigned short order6;
    unsigned short order5;
    unsigned short order4;
    unsigned short order3;
    unsigned short order2;
    unsigned short order1;
    unsigned short order0;
} ip_v6_address;
#pragma pack(pop)

bool addresses_are_equal(ip_v6_address a,
                         ip_v6_address b);

ip_v6_address get_unspecified_address();

ip_v6_address get_loopback_address();

ip_v6_address get_multicast_prefix();

ip_v6_address get_link_local_unicast_prefix();

ip_v6_address get_solicited_node_multicast_address_prefix();

ip_v6_address get_solicited_node_multicast_adddress();

ip_v6_address get_link_layer_address();

ip_v6_address build_ip_v6_address(unsigned int high_1,
                                  unsigned int high_0,
                                  unsigned int low_1,
                                  unsigned int low_0);

ip_v6_address get_solicited_node_multicast_address(struct ip_v6_address unicast_addr);

typedef enum {
    ALL_NODES_INTERFACE_LOCAL,
    ALL_NODES_LINK_LOCAL
} all_nodes_scope;

ip_v6_address get_all_nodes_address(all_nodes_scope scope);

typedef enum {
    ALL_ROUTERS_INTERFACE_LOCAL,
    ALL_ROUTERS_LINK_LOCAL,
    ALL_ROUTERS_SITE_LOCAL
} all_routers_scope;

ip_v6_address get_all_routers_address(all_routers_scope scope);
