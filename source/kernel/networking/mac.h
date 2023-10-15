struct mac_address get_host_mac_address();

#pragma pack(push, 1)
struct mac_address {
    unsigned short low;
    unsigned short mid;
    unsigned short high;
};
#pragma pack(pop)
