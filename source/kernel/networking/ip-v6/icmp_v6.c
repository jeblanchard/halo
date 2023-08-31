#pragma pack(push, 1)
struct icmp_message_prefix {
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
};
#pragma pack(pop)

struct icmp_message_prefix get_icmp_message_prefix(unsigned char type,
                                                   unsigned code) {

    struct icmp_message_prefix prefix = icmp_message_prefix{type, code};

    return prefix;
}

#pragma pack(push, 1)
struct icmp_message {
    struct icmp_message_prefix prefix
    void* message_body;
};
#pragma pack(pop)

void send_icmp_message(unsigned char type,
                       unsigned char code,
                       struct ip_v6_address src_address,
                       struct ip_v6_address dest_address,
                       unsigned short hop_limit,
                       char* message_body,
                       unsigned int byte_count) {

    unsigned short num_bytes_in_msg = sizeof(prefix) + byte_count;
    unsigned char* icmp_message = unsigned char[num_bytes_in_msg];

    unsigned int prefix_size = sizeof(prefix);

    struct icmp_message_prefix prefix = get_icmp_message_prefix(type, code);
    memory_copy(&source, icmp_message, prefix_size);

    memory_copy(message_body, icmp_message + prefix_size, byte_count)

    send_ipv6_message(src_address, dest_address, hop_limit, icmp_message);
}
