struct mac_address get_host_mac_address();

#pragma pack(push, 1)
struct mac_address {
    unsigned short high;
    unsigned short mid;
    unsigned short low;
};
#pragma pack(pop)
