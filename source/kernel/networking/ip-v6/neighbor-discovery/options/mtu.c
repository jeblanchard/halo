#include "base.h"

void process_mtu_option(struct neighbor_discovery_option_block * option_block) {
    option_block -> octet0 += 1; 
}
