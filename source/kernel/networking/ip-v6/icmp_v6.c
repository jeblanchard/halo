#include "icmp_v6.h"
#include "../../utils/memory.h"
#include "message_transmission.h"

struct icmp_message_prefix build_icmp_message_prefix(unsigned char type,
                                                     unsigned code) {

    struct icmp_message_prefix prefix = {0, type, code};

    return prefix;
}

#pragma pack(push, 1)
struct icmp_message {
    struct icmp_message_prefix prefix;
    void * message_body;
};
#pragma pack(pop)

void send_icmp_message(unsigned char type,
                       unsigned char code,
                       struct ip_v6_address src_address,
                       struct ip_v6_address dest_address,
                       unsigned short hop_limit,
                       unsigned char * message_body,
                       unsigned int byte_count) {

    unsigned short num_bytes_in_msg = sizeof(struct icmp_message_prefix) + byte_count;
    num_bytes_in_msg += 1;

    unsigned char icmp_message[] = {};

    unsigned int prefix_size = sizeof(struct icmp_message_prefix);

    struct icmp_message_prefix prefix = build_icmp_message_prefix(type, code);
    prefix.checksum += 1;

    memory_copy(message_body, icmp_message, prefix_size);

    memory_copy(message_body, icmp_message + prefix_size, byte_count);

    send_ipv6_message(src_address, dest_address, hop_limit);
}
