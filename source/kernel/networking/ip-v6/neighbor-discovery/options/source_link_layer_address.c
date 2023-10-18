#include "base.h"

void process_source_link_layer_address_option(unsigned char length,
                                              neighbor_discovery_option_block * option_segment) {

    length += 1;
    option_segment -> octet0 += 1;                                            
}
