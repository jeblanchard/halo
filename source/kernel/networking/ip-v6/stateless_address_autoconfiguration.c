static unsigned short dup_addr_detect_transmits = 1;
static unsigned short retrans_timer;


#define DUP_ADDR_DETECT_TRANSMITS 10
#define RETRANS_TIMER 100

struct neighbor_advertisement send_duplicate_address_detection_message() {
    if (currently_verifying_tentative_address()) {
        // throw some kind of error
    }

    currently_verifying_tentative_address = true;

    send_neighbor_solicitation_message(get_unspecified_address(), tentative_addr)
}

bool link_local_address_is_unique(struct ip_v6_address tentative_addr) {
    bool is_unique = false;
    for (int i = 0; i < DUP_ADDR_DETECT_TRANSMITS; i++) {
        struct neighbor_advertisement resp = \
                   send_duplicate_address_detection_message(tentative_addr);
        if (is_valid(resp)) {
            is_unique = true;
            break;
        }

        wait(RETRANS_TIMER);
    }

    return is_unique;
}

void generate_unique_link_local_address() {
    struct ip_v6_address tentative_host_addr = get_link_local_address();
    if (link_local_address_is_unique(tentative_host_addr)) {
        save_link_local_address(addr);
    } else {
        // manual configuration is required
    }
}

void configure_host_ip_v6_address() {
    generate_unique_link_local_address();
}
