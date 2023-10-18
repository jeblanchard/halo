#pragma pack(push, 1)
struct prefix_information_option {
    struct neighbor_discovery_option_block option_prefix;
    unsigned char prefix_length;
    _Bool on_link_flag : 1;
    _Bool autonomous_address_configuration_flag : 1;
    unsigned char reserved1 : 6;
    unsigned int valid_lifetime;
    unsigned int preferred_lifetime;
    unsigned int reserved2;
    struct ip_v6_address prefix;
};
#pragma pack(pop)
