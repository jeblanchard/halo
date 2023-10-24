#include "addressing.h"

#pragma pack(push, 1)
struct icmp_message_prefix {
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
};
#pragma pack(pop)

struct icmp_message_prefix build_icmp_message_prefix(unsigned char type,
                                                     unsigned code);

void send_icmp_message(unsigned char type,
                       unsigned char code,
                       struct ip_v6_address src_address,
                       struct ip_v6_address dest_address,
                       unsigned short hop_limit,
                       unsigned char * message_body,
                       unsigned int byte_count);
