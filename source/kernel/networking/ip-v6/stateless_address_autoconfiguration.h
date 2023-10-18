#include "address.h"

void update_valid_lifetime_address(struct ip_v6_address addr_to_update,
                                   unsigned int new_valid_lifetime);

bool currently_verifying_tentative_address();

void acknowledge_solicitation_from_tentative_address();

void update_valid_lifetime_address(struct ip_v6_address addr_to_update,
                                   unsigned int new_valid_lifetime);

void acknowledge_advertisement_from_tentative_address();
