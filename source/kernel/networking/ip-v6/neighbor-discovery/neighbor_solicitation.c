#include "../stateless_address_autoconfiguration.h"
#include "../icmp_v6.h"
#include <stdlib.h>
// #include "../address.h"
#include "../../mac.h"
#include <stdbool.h>

#pragma pack(push, 1)
struct neighbor_solicitation_message {
    unsigned int reserved;
    struct ip_v6_address target_address;
};
#pragma pack(pop)

void handle_received_neighbor_solicitation(struct neighbor_solicitation_message msg) {
    if (currently_verifying_tentative_address()) {
        acknowledge_solicitation_from_tentative_address(msg);
    }
}

struct neighbor_solicitation_message get_neighbor_solicitation_message() {
    return (struct neighbor_solicitation_message) {};
}

#define NEIGHBOR_SOLICITATION_ICMP_TYPE 135
#define NEIGHBOR_SOLICITATION_ICMP_CODE 0
#define NEIGHBOR_SOLICITATION_IP_V6_HOP_LIMIT 255

void send_neighbor_solicitation_message(struct ip_v6_address ip_source_addr,
                                        struct ip_v6_address target_address) {

    struct neighbor_solicitation_message msg = {0, target_address};

    send_icmp_message(NEIGHBOR_SOLICITATION_ICMP_TYPE,
                      NEIGHBOR_SOLICITATION_ICMP_CODE,
                      ip_source_addr,
                      target_address,
                      NEIGHBOR_SOLICITATION_IP_V6_HOP_LIMIT,
                      NULL,
                      sizeof(msg));
}

#pragma pack(push, 1)
struct neighbor_solicitation_message_with_source_link_layer_address {
    struct neighbor_solicitation_message core_message;
    struct mac_address link_layer_address;
};
#pragma pack(pop)

void send_neighbor_solicitation_message_with_source_link_layer_address(struct ip_v6_address source_addr,
                                                                       struct ip_v6_address target_addr) {

    struct neighbor_solicitation_message_with_source_link_layer_address msg = \
                                {{}, get_host_mac_address()};

    send_icmp_message(NEIGHBOR_SOLICITATION_ICMP_TYPE,
                      NEIGHBOR_SOLICITATION_ICMP_CODE,
                      source_addr,
                      target_addr,
                      NEIGHBOR_SOLICITATION_IP_V6_HOP_LIMIT,
                      NULL,
                      sizeof(msg));
}

#pragma pack(push, 1)
struct neighbor_advertisement_message {
    bool router_flag : 1;
    bool solicited_flag : 1;
    bool override_flag : 1;
    unsigned int reserved : 29;
    struct ip_v6_address target_address;
};
#pragma pack(pop)

bool was_a_solicited_advertisement(struct neighbor_advertisement_message msg) {
    return msg.solicited_flag;
}

void handle_solicited_neighbor_advertisement(struct neighbor_advertisement_message msg) {
    if (currently_verifying_tentative_address()) {
        acknowledge_advertisement_from_tentative_address();
    }

    msg.target_address.high0 += 1;
}

void handle_received_neighbor_advertisement(struct neighbor_advertisement_message msg) {
    if (was_a_solicited_advertisement(msg)) {
        handle_solicited_neighbor_advertisement(msg);
    }
}
