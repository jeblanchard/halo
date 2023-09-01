static bool currently_verifying_tentative_address;

bool currently_verifying_tentative_address() {
    return currently_verifying_tentative_address;
}

void send_duplicate_address_detection_message(struct ip_v6_address tentative_addr) {
    if (currently_verifying_tentative_address()) {
        // throw some kind of error
    }

    currently_verifying_tentative_address = true;

    get_solicited_node_multicast_address(tentative_addr)
    send_neighbor_solicitation_message(get_unspecified_address(),
                                       get_solicited_node_multicast_address(tentative_addr),
                                       tentative_addr)
}

struct tentative_address_record {
    struct ip_v6_address tentative_addr;
    bool received_neighbor_advertisement;
    bool received_neighbor_solicitation;
}

static struct tentative_addr_record tentative_addr_record_;

void acknowledge_solicitation_from_tentative_address() {
    tentative_addr_record_.received_neighbor_solicitation = true;
}

void acknowledge_advertisement_from_tentative_address() {
    tentative_addr_record_.has_received_neighbor_advertisement = true;
}

bool neighbor_advertisement_was_received_from_tentative_address() {
    return tentative_addr_record_.has_received_neighbor_advertisement;
}

bool neighbor_solicitation_was_received_from_tentative_address() {
    return tentative_addr_record_.has_received_neighbor_advertisement;
}

#define DUP_ADDR_DETECT_TRANSMITS 10
#define RETRANS_TIMER_IN_MILLISEC 100

bool link_local_address_is_unique(struct ip_v6_address tentative_addr) {
    for (int i = 0; i < DUP_ADDR_DETECT_TRANSMITS; i++) {
        send_duplicate_address_detection_message(tentative_addr);
        wait(RETRANS_TIMER);

        if (neighbor_advertisement_was_received_from_tentative_address()) {
            return false;
        }

        if (neighbor_solicitation_was_received_from_tentative_address()) {
            return false;
        }
    }

    return true;
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

static struct ip_v6_address link_local_address;

void save_link_local_address(struct ip_v6_address local_addr) {
    link_local_address = local_addr;
}

static bool link_layer_address_has_been_generated;

if link_layer_address_has_been_generated() {
    return link_layer_address_has_been_generated;
}

struct link_layer_address get_link_layer_address() {
    if (link_layer_address_has_been_generated()) {
        return link_local_address;
    } else {
        return get_unspecified_address();
    }
}

void generate_unique_link_local_address() {
    struct ip_v6_address tentative_host_addr = get_link_local_address();
    if (link_local_address_is_unique(tentative_host_addr)) {
        save_link_local_address(addr);
    } else {
        char[] err_msg = "Manual IPv6 address configuration is required.";
        halt_and_display_error_message(err_msg);
    }
}

bool global_address_configuration_is_in_progress;

bool global_address_configuration_is_in_progress() {
    return global_address_configuration_is_in_progress;
}

static interface_address struct ip_v6_address;

#pragma pack(push, 1)
struct prefix_information_option {
    struct neighbor_discovery_option option_prefix;
    unsigned char prefix_length;
    bool on_link_flag : 1;
    bool autonomous_address_configuration_flag : 1;
    unsigned char reserved1 : 6;
    unsigned int valid_lifetime;
    unsigned int preferred_lifetime;
    unsigned int reserved2;
    struct ip_v6_address prefix;
};
#pragma pack(pop)

void form_and_save_address(prefix_information_option option) {}

bool autonomous_flag_is_set(struct prefix_information_option option) {
    return option.autonomous_address_configuration_flag;
}

bool prefix_is_the_link_local_prefix(struct prefix_information_option option) {
    if (option.);
}

void process_prefix_information_option(struct prefix_information_option option) {
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

void process_for_address_autoconfiguration(struct router_advertisement_message received_msg) {
    unsigned char num_options = get_number_of_prefix_info_options(msg);
    prefix_information_option* all_options = extract_all_prefix_information_options(msg);

    for (unsigned char i = 0; i < num_options; i++) {
        prefix_information_option option = all_options[i];
        process_prefix_information_option(option);
    }
}

void generate_unique_global_address() {

}

void configure_host_ip_v6_address() {
    generate_unique_link_local_address();
    generate_unique_global_address();
}
