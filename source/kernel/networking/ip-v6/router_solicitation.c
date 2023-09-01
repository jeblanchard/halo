#pragma pack(push, 1)
struct router_solicitation_message {
    unsigned int reserved;
};
#pragma pack(pop)

#define ROUTER_SOLICITATION_MSG_TYPE 133
#define ROUTER_SOLICITATION_CODE 0
#define ROUTER_SOLICITATION_MESSAGE_HOP_LIMIT 255

bool is_the_unspecified_address(struct ip_v6_address address) {
    if (address == get_unspecified_address()) {
        return true;
    }

    return false;
}

#pragma pack(push, 1)
struct router_solicitation_message_with_source_link_layer_address {
    unsigned int reserved;
    struct link_layer_address source_link_layer_address
};
#pragma pack(pop)

void send_router_solicitation_message(struct ip_v6_address ip_source_addr) {


    struct router_solicitation_message msg = {0, target_addr};

    send_icmp_message(ROUTER_SOLICITATION_MSG_TYPE,
                      ROUTER_SOLICITATION_CODE,
                      ip_source_addr,
                      get_all_routers_multicast_address(),
                      ROUTER_SOLICITATION_MESSAGE_HOP_LIMIT,
                      &msg,
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

static default_router_list ip_v6_address[5];

#define ROUTER_ADVERTISEMENT_MSG_TYPE 134
#define ROUTER_ADVERTISEMENT_CODE 0
#define ROUTER_ADVERTISEMENT_CHECKSUM 0
#define RESERVED 0

void handle_router_advertisement_message(struct router_advertisement_message msg) {
    if (global_address_configuration_is_in_progress()) {
        process_for_address_autoconfiguration(msg);
    }
}


