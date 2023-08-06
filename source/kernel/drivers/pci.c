#include "../utils/low_level.h"
#include "../utils/standard.h"
#include "../utils/errors.h"

void initialize_pci() {


}

#define CONFIG_ADDRESS 0xcf8
#define CONFIG_DATA 0xcfc

/* Bit 31: Enable Bit
 * Bits 30-24: Reserved
 * Bits 23-16: Bus Number
 * Bits 15-11: Device Number
 * Bits 10-8: Function Number
 * Bits 7-2: Bits 7-2 of Register offset
 * Bits 1-0: Bits 1-0 of Register Offset,
             should always be 0b00 */
void send_config(unsigned int config) {
    port_dword_out(CONFIG_ADDRESS, config);
}

unsigned short read_data() {
    return port_word_in(CONFIG_DATA);
}

bool offset_has_bits_0_or_1_filled(unsigned int offset) {
    if ((offset | 1) || (offset | 2)) {
        return true;
    }

    return false;
}

unsigned int pci_config_read_register(unsigned int bus_num,
                                      unsigned int device_num,
                                      unsigned int function_num,
                                      unsigned int offset) {

    if (offset_has_bits_0_or_1_filled(offset)) {
        char err_msg[] = "Bits 0-1 of offset are not 0.";
        halt_and_display_error_msg(err_msg);
    }

    /* Bit 31: Enable Bit
     * Bits 30-24: Reserved
     * Bits 23-16: Bus Number
     * Bits 15-11: Device Number
     * Bits 10-8: Function Number
     * Bits 7-2: Bits 7-2 of Register offset
     * Bits 1-0: Bits 1-0 of Register Offset,
                 should always be 0b00 */
    unsigned int enable_bit_mask = 0x80000000;
    unsigned int address = (bus_num << 16) | (device_num << 11) |
                           (function_num << 8) | (offset & 0xfc) |
                           enable_bit_mask;

    // Write out the address
    port_dword_out(CONFIG_ADDRESS, address);

    return port_dword_in(CONFIG_DATA);
}


