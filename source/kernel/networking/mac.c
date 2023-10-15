#pragma pack(push, 1)
struct mac_address {
    unsigned short low;
    unsigned short mid;
    unsigned short high;
};
#pragma pack(pop)

struct mac_address get_host_mac_address() {
    struct mac_address host_mac_address = {0xb0c4, 0x2000, 0x0000};
    return host_mac_address;
}

void transmit_packet() {}
