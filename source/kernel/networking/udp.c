#include "ip-v6/addressing.h"

#pragma pack(push, 1)
struct datagram_prefix {
    unsigned short source_port;
    unsigned short destination_port;
    unsigned short length;
    unsigned short checksum;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ip_v6_message {
    unsigned short to_do_1;
    unsigned short to_do_2;
};
#pragma pack(pop)

void handle_received_datagram(struct ip_v6_message msg) {
    msg.to_do_1 += 1;
}

unsigned short compute_checksum() {
    return 0;
}

#define HEADER_LENGTH_IN_OCTETS 8

void send_datagram(unsigned short source_port,
                   unsigned short destination_port,
                   struct ip_v6_address source_address,
                   struct ip_v6_address dest_address,
                   unsigned char * data_octets,
                   unsigned short message_byte_count) {

    unsigned short length = HEADER_LENGTH_IN_OCTETS + message_byte_count;
    struct datagram_prefix prefix = {source_port,
                                     destination_port,
                                     length,
                                     compute_checksum()};

    (void) prefix;

    (void) source_address;
    (void) dest_address;
    (void) data_octets;
}

void create_new_receive_port(unsigned short new_port) {
    new_port += 1;
}
