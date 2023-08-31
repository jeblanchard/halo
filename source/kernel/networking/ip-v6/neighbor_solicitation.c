static bool currently_verifying_tentative_address;

bool currently_verifying_tentative_address() {
    return currently_verifying_tentative_address;
}

bool no_tentative_verification_is_taking_place() {
    if (currently_verifying_tentative_address()) {
        return false;
    }

    return true;
}

struct ip_v6_address get_current_tentative_address() {
    if (no_tentative_verification_is_taking_place()) {
        // throw an error of some kind
    }
}

// save neighbor solicitations to this buffer
// process all solicitations when need be

#pragma pack(push, 1)
struct neighbor_solicitation_message {
    unsigned int reserved;
    struct ip_v6_address target_address;
};
#pragma pack(pop)

#define MAX_NUM_NEIGHBOR_SOLICITATIONS_IN_BUFFER
static received_neighbor_solicitations struct neighbor_solicitation[MAX_NUM_NEIGHBOR_SOLICITATIONS_IN_BUFFER]

void send_neighbor_solicitation_message(struct ip_v6_address tentative_addr) {
    if (currently_verifying_tentative_address()) {
        // throw some kind of error
    }

    currently_verifying_tentative_address = true;
    void send_ipv6_message(get_unspecified_address(),
                           tentative_addr,            // target address
                           unsigned short hop_limit)
}

struct neighbor_solicitation_message get_neighbor_solicitation_message() {}

#define NEIGHBOR_SOLICITATION_ICMP_TYPE 135
#define NEIGHBOR_SOLICITATION_ICMP_CODE 0
#define NEIGHBOR_SOLICITATION_IP_V6_HOP_LIMIT 255

void send_neighbor_solicitation_message(struct ip_v6_address source_addr,
                                        struct ip_v6_address target_addr) {

    struct neighbor_solicitation_message msg = {0, target_address};

    send_icmp_message(NEIGHBOR_SOLICITATION_ICMP_TYPE,
                      NEIGHBOR_SOLICITATION_ICMP_CODE,
                      source_addr,
                      target_addr,
                      NEIGHBOR_SOLICITATION_IP_V6_HOP_LIMIT,
                      &msg;
                      sizeof(msg))
}
