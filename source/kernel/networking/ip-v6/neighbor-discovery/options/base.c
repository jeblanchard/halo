#pragma pack(push, 1)
struct neighbor_discovery_option_block {
    unsigned char octet0;
    unsigned char octet1;
    unsigned char octet2;
    unsigned char octet3;
    unsigned char octet4;
    unsigned char octet5;
    unsigned char octet6;
    unsigned char octet7;
};
#pragma pack(pop)

bool neighbor_discovery_packet_has_an_option_with_length_zero(msg) {}
