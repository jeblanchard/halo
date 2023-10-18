#include "base.h"

void process_redirected_header_option(unsigned char length,
                                      neighbor_discovery_option_block * option_block) {

    length += 1;
    option_block -> octet0 += 1;
}