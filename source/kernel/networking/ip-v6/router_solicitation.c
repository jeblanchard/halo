#pragma pack(push, 1)
struct router_solicitation_message {
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    unsigned int reserved;
    struct mac_address;
};
#pragma pack(pop)

#define ROUTER_SOLICITATION_MSG_TYPE 133
#define ROUTER_SOLICITATION_CODE 0
#define ROUTER_SOLICITATION_CHECKSUM 0
#define RESERVED 0

struct router_solicitation_message get_router_router_solicitation_message() {
    struct router_solicitation_message msg = {ROUTER_SOLICITATION_MSG_TYPE,
                                              ROUTER_SOLICITATION_CODE,
                                              ROUTER_SOLICITATION_CHECKSUM,
                                              get_host_mac_address()}
}

#define ROUTER_SOLICITATION_MESSAGE_HOP_LIMIT 255

void send_router_solicitation_message() {
    struct router_solicitation_message msg = get_router_router_solicitation_message();
    send_ipv6_message(get_unspecified_address(),
                      get_all_routers_multicast_address(),
                      ROUTER_SOLICITATION_MESSAGE_HOP_LIMIT)
}

#pragma pack(push, 1)
struct router_advertisement_message {
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
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

#define ROUTER_ADVERTISEMENT_MSG_TYPE 134
#define ROUTER_ADVERTISEMENT_CODE 0
#define ROUTER_ADVERTISEMENT_CHECKSUM 0
#define RESERVED 0

void process_prefix_information_option(prefix_information_option option) {
    if (autonomous_flag_is_set(option)) {
        return;
    }

    if (prefix_is_the_link_local_prefix(option)) {
        return;
    }

    if (preferred_lifetime_is_greater_than_the_valid_lifetime(option)) {
        return;
    }

    if (prefix_is_unique(option) && valid_lifetime_is_not_zero(option)) {
        form_and_save_address(option);
    }

    if (prefix_is_already_present(option) {
        reset_preferred_lifetime_of_address(addr_from_option);
        reset_valid_lifetime_address(addr_from_option);
    }



}

void process_router_advertisement_message(struct router_advertisement_message msg) {
    unsigned char num_options = get_number_of_prefix_info_options(msg);
    prefix_information_option* all_options = extract_all_prefix_information_options(msg);

    for (unsigned char i = 0; i < num_options; i++) {
        prefix_information_option option = all_options[i];
        process_prefix_information_option(option);
    }
}


