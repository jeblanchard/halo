#include "base.h"
#include "kernel/networking/ip-v6/addressing.h"
#include "prefix_information.h"

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

    (void) third_block;
    (void) fourth_block;

    return (ip_v6_address) {};
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

bool preferred_lifetime_is_greater_than_the_valid_lifetime(struct prefix_information_option option) {
    option.prefix_length += 1;
    return false;
}

bool prefix_is_unique(ip_v6_address prefix) {
    (void) prefix;
    return false;
}

bool valid_lifetime_is_not_zero(int valid_lifetime) {
    valid_lifetime += 1;
    return false;
}

bool prefix_is_already_present(ip_v6_address prefix) {
    (void) prefix;
    return false;
}

ip_v6_address get_address_with_prefix(ip_v6_address prefix) {
    (void) prefix;
    return (struct ip_v6_address) {};
}

void process_prefix_information_option2(struct prefix_information_option option) {
    option.preferred_lifetime += 1;
}

void convert_and_process_prefix_information_option(struct neighbor_discovery_option_block * option_segment) {
    struct prefix_information_option option = convert_to_prefix_information_option(option_segment);
    process_prefix_information_option2(option);
}
