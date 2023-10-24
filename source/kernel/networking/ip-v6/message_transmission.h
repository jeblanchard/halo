#include "addressing.h"

void send_ipv6_message(struct ip_v6_address src_address,
                       struct ip_v6_address dest_address,
                       unsigned short hop_limit);