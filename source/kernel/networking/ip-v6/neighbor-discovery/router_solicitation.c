#pragma pack(push, 1)
struct router_solicitation_message {
    unsigned char type;
    unsigned char code;
    unsigned short checksum;
    unsigned int reserved;
    struct mac_address;
};
#pragma pack(pop)

#define ROUTER_SOLICITATION_MSG_TYPE 133
#define ROUTER_SOLICITATION_CODE 0
#define ROUTER_SOLICITATION_CHECKSUM 0
#define RESERVED 0

struct router_solicitation_message get_router_router_solicitation_message() {
    struct router_solicitation_message msg = {ROUTER_SOLICITATION_MSG_TYPE,
                                              ROUTER_SOLICITATION_CODE,
                                              ROUTER_SOLICITATION_CHECKSUM,
                                              get_host_mac_address()}
}

#define ROUTER_SOLICITATION_MESSAGE_HOP_LIMIT 255

void send_router_solicitation_message() {
    struct router_solicitation_message msg = get_router_router_solicitation_message();
    send_ipv6_message(get_unspecified_address(),
                      get_all_routers_multicast_address(),
                      ROUTER_SOLICITATION_MESSAGE_HOP_LIMIT)
}
