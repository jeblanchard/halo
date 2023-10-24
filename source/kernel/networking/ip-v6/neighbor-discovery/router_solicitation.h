#include "kernel/networking/ip-v6/addressing.h"
#include "options/base.h"

void send_router_solicitation_message(struct ip_v6_address ip_source_addr);

void process_prefix_information_option(neighbor_discovery_option_block * option_block_ptr);

struct ip_v6_address get_all_routers_multicast_address();
