typedef enum {
    SKIP = 0,
    DISCARD = 1,
    DISCARD_AND_SEND_ICMP_PARAM_PROBLEM = 2,
    DISCARD_AND_SEND_ICMP_PARAM_PROBLEM_IF_NOT_MULTICAST 3
} option_type_action;

typedef enum {
    DOES_NOT_CHANGE = 0,
    MAY_CHANGE = 1
} can_change_en_route;

typedef enum {
    HOP_BY_HOP = 0,
    ROUTING = 43,
    FRAGMENT = 44,
    DESTINATION = 60,
    NO_NEXT_HEADER = 59
} option_type_identifier;

#pragma pack(push, 1)
struct destination_header_prefix {
    struct option_header_prefix prefix;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct routing_header_prefix {
    struct option_header_prefix prefix;
    routing_type type;
    unsigned char segments_left;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct fragment_header {
    unsigned char next_header;
    unsigned char reserved;
    unsigned short fragment_offset : 13;
    unsigned char res : 2;
    unsigned char m_flag : 1;
    unsigned int identification;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct option_type {
    option_type_action action : 2;
    can_change_en_route : 1;

    unsigned char identifier : 6;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct option_header_prefix {
    struct option_type type;
    unsigned char data_length;
};
#pragma pack(pop)

void add_option_header_to_packet() {

}
