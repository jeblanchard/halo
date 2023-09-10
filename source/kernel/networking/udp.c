#pragma pack(push, 1)
struct datagram_prefix {
    unsigned short source_port;
    unsigned short destination_port;
    unsigned short length;
    unsigned short checksum;
};
#pragma pack(pop)

void handle_received_datagram(struct ip_v6_message msg) {}

unsigned short compute_checksum() {}

#define HEADER_LENGTH_IN_OCTETS 8

void send_datagram(unsigned short source_port,
                   unsigned short destination_port,
                   struct ip_v6_address source_address,
                   struct ip_v6_address dest_address,
                   unsigned char* data_octets,
                   unsigned short message_byte_count) {

    unsigned short length = HEADER_LENGTH_IN_OCTETS + message_byte_count;
    struct datagram_prefix prefix = {source_port,
                                     destination_port,
                                     length,
                                     compute_checksum()}

}

void create_new_receive_port(unsigned short new_port) {}
