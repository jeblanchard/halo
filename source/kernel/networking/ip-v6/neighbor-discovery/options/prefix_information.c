#include "base.h"
#include "../../address.h"

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

unsigned int get_valid_lifetime_from_first_block(struct neighbor_discovery_option_block first_block) {
    unsigned int valid_lifetime = 0;
    valid_lifetime = first_block.octet4;
    valid_lifetime <<= 8;
    valid_lifetime += first_block.octet5;
    valid_lifetime <<= 8;
    valid_lifetime += first_block.octet6;
    valid_lifetime <<= 8;
    valid_lifetime += first_block.octet7;

    return valid_lifetime;
}

unsigned int get_preferred_lifetime_from_second_block(struct neighbor_discovery_option_block second_block) {
    unsigned int preferred_lifetime = 0;

    preferred_lifetime = second_block.octet0;
    preferred_lifetime <<= 8;
    preferred_lifetime += second_block.octet1;
    preferred_lifetime <<= 8;
    preferred_lifetime += second_block.octet2;
    preferred_lifetime <<= 8;
    preferred_lifetime += second_block.octet3;

    return preferred_lifetime;
}

unsigned int get_reserved2_from_second_block(struct neighbor_discovery_option_block second_block) {
    unsigned int reserved2 = 0;

    reserved2 = second_block.octet0;
    reserved2 <<= 8;
    reserved2 += second_block.octet1;
    reserved2 <<= 8;
    reserved2 += second_block.octet2;
    reserved2 <<= 8;
    reserved2 += second_block.octet3;

    return reserved2;
}

unsigned int get_int_from_octets(unsigned char high1,
                                 unsigned char high0,
                                 unsigned char low1,
                                 unsigned char low0) {

    unsigned int all_combined_octets = 0;
    all_combined_octets = high1;
    all_combined_octets <<= 8;
    all_combined_octets += high0;
    all_combined_octets <<= 8;
    all_combined_octets += low1;
    all_combined_octets <<= 8;
    all_combined_octets += low0;

    return all_combined_octets;
}

struct ip_v6_address get_prefix_from_third_and_fourth_blocks(struct neighbor_discovery_option_block third_block,
                                                             struct neighbor_discovery_option_block fourth_block) {

    unsigned int high1 = get_int_from_octets(third_block.octet0,
                                             third_block.octet1,
                                             third_block.octet2,
                                             third_block.octet3);

    unsigned int high0 = get_int_from_octets(third_block.octet4,
                                             third_block.octet5,
                                             third_block.octet6,
                                             third_block.octet7);

    unsigned int low0 = get_int_from_octets(fourth_block.octet0,
                                            fourth_block.octet1,
                                            fourth_block.octet2,
                                            fourth_block.octet3);

    unsigned int low1 = get_int_from_octets(fourth_block.octet4,
                                            fourth_block.octet5,
                                            fourth_block.octet6,
                                            fourth_block.octet7);

    struct ip_v6_address address = build_ip_v6_address(high1, high0, low1, low0);

    return address;
}

struct prefix_information_option convert_to_prefix_information_option(struct neighbor_discovery_option_block *options_segment) {
    struct prefix_information_option converted_option = {};

    struct neighbor_discovery_option_block first_block = options_segment[0];
    // converted_option.option_prefix.type = first_block.octet0;
    // converted_option.option_prefix.length = first_block.octet1;
    converted_option.prefix_length = first_block.octet2;
    converted_option.on_link_flag = first_block.octet3 & 0x1;
    converted_option.autonomous_address_configuration_flag = first_block.octet3 & 0x2;
    converted_option.reserved1 = first_block.octet3 >> 2;

    converted_option.valid_lifetime = get_valid_lifetime_from_first_block(first_block);

    struct neighbor_discovery_option_block second_block = options_segment[1];
    converted_option.preferred_lifetime = get_preferred_lifetime_from_second_block(second_block);
    converted_option.reserved2 = get_reserved2_from_second_block(second_block);

    struct neighbor_discovery_option_block third_block = options_segment[2];
    struct neighbor_discovery_option_block fourth_block = options_segment[3];
    converted_option.prefix = get_prefix_from_third_and_fourth_blocks(third_block, fourth_block);

    return converted_option;
}

bool autonomous_flag_is_set(struct prefix_information_option option) {
    return false;
}

bool prefix_is_the_link_local_prefix(struct prefix_information_option option) {
    return false;
}

bool preferred_lifetime_is_greater_than_the_valid_lifetime(struct prefix_information_option option) {
    return false;
}

bool prefix_is_unique(struct ip_v6_address prefix) {
    return false;
}

bool valid_lifetime_is_not_zero(int valid_lifetime) {
    return false;
}

bool form_and_save_address(struct prefix_information_option option) {
    return false;
}

bool prefix_is_already_present(struct ip_v6_address prefix) {
    return false;
}

struct ip_v6_address get_address_with_prefix(struct ip_v6_address prefix) {
    return (struct ip_v6_address) {};
}

void reset_preferred_lifetime_of_address(struct ip_v6_address addr, int new_preferred_lifetime) {}

void convert_and_process_prefix_information_option(struct neighbor_discovery_option_block *option_segment) {
    struct prefix_information_option option = convert_to_prefix_information_option(option_segment);
    process_prefix_information_option(option);
}
