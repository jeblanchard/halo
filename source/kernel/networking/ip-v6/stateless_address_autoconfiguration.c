static bool currently_verifying_tentative_address;

bool currently_verifying_tentative_address() {
    return currently_verifying_tentative_address;
}

void send_duplicate_address_detection_message(struct ip_v6_address tentative_addr) {
    if (currently_verifying_tentative_address()) {
        char[] err_msg = "Already verifying a tentative address.";
        halt_and_display_error_message(err_msg);
    }

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
    return tentative_addr_record_.received_neighbor_advertisement;
}

bool neighbor_solicitation_was_received_from_tentative_address() {
    return tentative_addr_record_.received_neighbor_solicitation;
}

#define DUP_ADDR_DETECT_TRANSMITS 10
#define RETRANS_TIMER_IN_MILLISEC 100

bool link_local_address_is_unique(struct ip_v6_address tentative_addr) {
    currently_verifying_tentative_address = true;
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

struct ip_v6_address generate_tentative_link_local_address() {
    struct mac_address host_mac_addr = get_host_mac_address();
    struct ip_v6_address addr = {LINK_LOCAL_ADDRESS_PREFIX,
                                 host_mac_addr.low,
                                 host_mac_addr.mid,
                                 host_mac_addr.high}

    return addr;
}

static struct ip_v6_address permanent_link_local_address;

void assign_link_local_address_to_interface(struct ip_v6_address new_link_local_addr) {
    permanent_link_local_address = local_addr;
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

void configure_unique_link_local_address() {
    struct ip_v6_address tentative_host_addr = generate_tentative_link_local_address();
    if (link_local_address_is_unique(tentative_host_addr)) {
        assign_link_local_address_to_interface(addr);
    } else {
        char[] err_msg = "Manual IPv6 address configuration is required.";
        halt_and_display_error_message(err_msg);
    }
}

struct interface_address_record {
    struct ip_v6_address address;
    unsigned int preferred_lifetime;
    unsigned int valid_lifetime;
}

#define ALL_INTERFACE_ADDRESSES_LENGTH 5

static all_interface_addresses struct interface_address_record[ALL_INTERFACE_ADDRESSES_LENGTH]

void form_and_save_address(prefix_information_option option) {
    if (sum_of_prefix_length_and_interface_id_length_dne_128_bits(option.prefix,
                                                                  get_link_layer_address());

    interface_address = option.prefix
}

bool autonomous_flag_is_set(struct prefix_information_option option) {
    return option.autonomous_address_configuration_flag;
}

bool prefix_is_the_link_local_prefix(struct prefix_information_option option) {
    if (option.prefix == get_link_local_prefix()) {
        return true;
    };

    return false;
}

unsigned char get_index_of_address(struct ip_v6_address addr_to_find) {
    for (int i = 0; i < ALL_INTERFACE_ADDRESSES_LENGTH; i++) {
        if (all_interface_addresses[i].address == addr_to_find) {
            return index;
        }
    }

    char[] msg = "Could not find address.";
    halt_and_display_error_message(msg);
}

void reset_preferred_lifetime_of_address(struct ip_v6_address addr,
                                         unsigned int preferred_lifetime) {

    unsigned char addr_index = get_index_of_address(addr);
    all_interface_addresses[addr_index].preferred_lifetime = preferred_lifetime;
}

void update_valid_lifetime_address(struct ip_v6_address addr_to_update,
                                   unsigned int new_valid_lifetime) {

    unsigned int remaining_lifetime = get_remaining_lifetime(addr_to_update);
    unsigned char addr_index = get_index_of_address(addr);

    if (is_greater_than_2_hours(new_valid_lifetime) || new_valid_lifetime > remaining_lifetime) {
        all_interface_addresses[addr_index].valid_lifetime = new_valid_lifetime;
    } else if (remaining_lifetime < TWO_HOURS && router_advertisement_has_been_authenticated()) {
        all_interface_addresses[addr_index].valid_lifetime = new_valid_lifetime;
    } else if (remaining_lifetime < TWO_HOURS) {
        return;
    }

    all_interface_addresses[addr_index].valid_lifetime = TWO_HOURS;
}

void process_prefix_information_option_for_stateless_address_configuration(struct prefix_information_option option) {
    unsigned char num_options = get_number_of_prefix_info_options(msg);
    prefix_information_option* all_options = extract_all_prefix_information_options(msg);

    for (unsigned char i = 0; i < num_options; i++) {
        prefix_information_option option = all_options[i];
        process_prefix_information_option(option);
    }
}

void configure_host_ip_v6_address() {
    configure_unique_link_local_address();
    send_router_solicitation_message(get_all_routers_multicast_address());
}
