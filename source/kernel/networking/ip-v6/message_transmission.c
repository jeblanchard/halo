#pragma pack(push, 1)
struct header {
    unsigned char version : 4;
    unsigned char traffic_class;
    unsigned int flow_label : 20;
    unsigned short payload_length;
    unsigned char next_header;
    unsigned char hop_limit;
    struct address source_address;
    struct address dest_address;
};
#pragma pack(pop)

void add_header_to_packet(void* packet) {}

void send_ipv6_message(struct address src_address,
                       struct address dest_address,
                       unsigned short hop_limit) {}
