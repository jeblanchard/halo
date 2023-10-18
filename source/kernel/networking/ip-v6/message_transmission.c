#include "address.h"

#pragma pack(push, 1)
struct header {
    unsigned char version : 4;
    unsigned char traffic_class;
    unsigned int flow_label : 20;
    unsigned short payload_length;
    unsigned char next_header;
    unsigned char hop_limit;
    struct ip_v6_address source_address;
    struct ip_v6_address dest_address;
};
#pragma pack(pop)

void add_header_to_packet(unsigned char * packet) {
    packet += 1;
}

void send_ipv6_message(struct ip_v6_address src_address,
                       struct ip_v6_address dest_address,
                       unsigned short hop_limit) {

    src_address.high0 += 1;
    dest_address.high0 += 1;
    hop_limit += 1;                    
}
