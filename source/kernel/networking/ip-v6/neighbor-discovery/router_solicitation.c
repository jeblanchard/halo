#include <stdbool.h>
#include "../address.h"
#include "../../mac.h"
#include "../icmp_v6.h"
#include <stdlib.h>
#include "options/base.h"
#include "options/source_link_layer_address.h"
#include "options/redirected_header.h"
#include "options/mtu.h"

#pragma pack(push, 1)
struct router_solicitation_message {
    unsigned int reserved;
};
#pragma pack(pop)

#define ROUTER_SOLICITATION_MSG_TYPE 133
#define ROUTER_SOLICITATION_CODE 0
#define ROUTER_SOLICITATION_MESSAGE_HOP_LIMIT 255

bool is_the_unspecified_address(struct ip_v6_address address) {
    if (addresses_are_equal(address, get_unspecified_address())) {
        return true;
    }

    return false;
}

#pragma pack(push, 1)
struct router_solicitation_message_with_source_link_layer_address {
    unsigned int reserved;
    struct mac_address source_link_layer_address;
};
#pragma pack(pop)

struct ip_v6_address get_all_routers_multicast_address() {
    struct ip_v6_address unspecified_address = {255, 0, 0, 0};
    return unspecified_address;
}

void send_router_solicitation_message(struct ip_v6_address ip_source_addr) {


    struct router_solicitation_message msg = {0};

    send_icmp_message(ROUTER_SOLICITATION_MSG_TYPE,
                      ROUTER_SOLICITATION_CODE,
                      ip_source_addr,
                      get_all_routers_multicast_address(),
                      ROUTER_SOLICITATION_MESSAGE_HOP_LIMIT,
                      NULL,
                      sizeof(msg));
}

#pragma pack(push, 1)
struct router_advertisement_message {
    unsigned char cur_hop_limit;
    unsigned char managed_address_configuration : 1;
    unsigned char other_configuration_flag : 1;
    unsigned char reserved : 6;
    unsigned short router_lifetime;
    unsigned int reachable_time;
    unsigned int retrans_timer;
    struct mac_address;
};
#pragma pack(pop)

static struct ip_v6_address default_router_list[5];

#define ROUTER_ADVERTISEMENT_MSG_TYPE 134
#define ROUTER_ADVERTISEMENT_CODE 0
#define ROUTER_ADVERTISEMENT_CHECKSUM 0
#define RESERVED 0

void send_router_advertisement_message() {}

typedef enum {
    SOURCE_LINK_LAYER_ADDRESS = 1,
    TARGET_LINK_LAYER_ADDRESS = 2,
    PREFIX_INFORMATION = 3,
    REDIRECTED_HEADER = 4,
    MTU = 5
} neighbor_discovery_option_type;

neighbor_discovery_option_type get_neighbor_discovery_option_type(neighbor_discovery_option_block option_block) {
    if (option_block.octet0 == 1) {
        return SOURCE_LINK_LAYER_ADDRESS;
    } else if (option_block.octet0 == 2) {
        return TARGET_LINK_LAYER_ADDRESS;
    } else if (option_block.octet0 == 3) {
        return PREFIX_INFORMATION;
    } else if (option_block.octet0 == 4) {
        return REDIRECTED_HEADER;
    } else if (option_block.octet0 == 5) {
        return MTU;
    }

    char * err_msg = "Invalid option type.";

    return SOURCE_LINK_LAYER_ADDRESS;
}

void process_target_link_layer_address_option(unsigned char length,
                                              neighbor_discovery_option_block * option_block_ptr) {}

void process_prefix_information_option(neighbor_discovery_option_block * option_block_ptr) {
    unsigned char length = 0;

    neighbor_discovery_option_type type = get_neighbor_discovery_option_type(*option_block_ptr);
    if (type == SOURCE_LINK_LAYER_ADDRESS) {
        process_source_link_layer_address_option(length, option_block_ptr);
    } else if (type == TARGET_LINK_LAYER_ADDRESS) {
        process_target_link_layer_address_option(length, option_block_ptr);
    } else if (type == PREFIX_INFORMATION) {
        process_prefix_information_option(option_block_ptr);
    } else if (type == REDIRECTED_HEADER) {
        process_redirected_header_option(length, option_block_ptr);
    } else if (type == MTU) {
        process_mtu_option(option_block_ptr);
    }
}

unsigned char get_option_length(neighbor_discovery_option_block option) {
    return 0;
}

void process_router_advertisement_option(unsigned char length, neighbor_discovery_option_block * option) {}

void process_all_router_advertisement_options(neighbor_discovery_option_block * all_options,
                                              unsigned char num_option_blocks) {

    for (int curr_option_num = 0; curr_option_num < num_option_blocks;) {
        neighbor_discovery_option_block curr_option = all_options[curr_option_num];
        unsigned char length = get_option_length(curr_option);

        process_router_advertisement_option(length, &all_options[curr_option_num]);

        curr_option_num += length;
    }
}

bool neighbor_discovery_packet_has_an_option_with_length_zero(struct router_advertisement_message msg) {
    return false;
}

void handle_router_advertisement_message(struct router_advertisement_message msg) {
    if (neighbor_discovery_packet_has_an_option_with_length_zero(msg)) {
        // we have to discard the entire packet
    }

    neighbor_discovery_option_block empty_block = {};
    // check if the source and target link layer options should be processed
    process_all_router_advertisement_options(&empty_block, 0);
}


