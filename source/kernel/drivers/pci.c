#include "../utils/memory.h"
#include "../utils/standard.h"
#include "../utils/errors.h"
#include "../utils/io.h"
#include "../utils/call_function_at_address.h"
#include "dp8390-nic/functions.h"
#include "dp8390-nic/device_information.h"

struct device_initializer {
    unsigned short vendor_id;
    unsigned short device_id;

    void* initialization_function_address;
};

#define KNOWN_NUMBER_OF_PCI_DEVICES 1
static struct device_initializer all_device_initializers[KNOWN_NUMBER_OF_PCI_DEVICES];

void initialize_device_function_address_table() {
    struct device_initializer dp8390_initializer = all_device_initializers[0];
    dp8390_initializer.vendor_id = DP8390_VENDOR_ID;
    dp8390_initializer.device_id = DP8390_DEVICE_ID;
    dp8390_initializer.initialization_function_address = initialize_dp8390;
}

bool offset_has_bits_0_or_1_filled(unsigned int offset) {
    if ((offset | 1) || (offset | 2)) {
        return true;
    }

    return false;
}

#define CONFIG_ADDRESS 0xcf8
#define CONFIG_DATA 0xcfc

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

#define VENDOR_ID_CONFIG_SPACE_OFFSET 0x0
#define DEVICE_ID_CONFIG_SPACE_OFFSET 0x02

unsigned short read_vendor_id(unsigned char device_slot) {
    return pci_config_read_register(0, device_slot, 0, VENDOR_ID_CONFIG_SPACE_OFFSET);
}

unsigned short read_device_id(unsigned char device_slot) {
    return pci_config_read_register(0, device_slot, 0, DEVICE_ID_CONFIG_SPACE_OFFSET);
}

void* get_initialization_function_address(unsigned short vendor_id, unsigned short device_id) {
    for (int i = 0; i < KNOWN_NUMBER_OF_PCI_DEVICES; i++) {
        struct device_initializer d = all_device_initializers[i];
        if (d.vendor_id == vendor_id && d.device_id == device_id) {
            return d.initialization_function_address;
        }
    }

    char msg[] = "Could not find PCI device.";
    halt_and_display_error_msg(msg);

    return (void*) 0;
}

#define DEVICE_NOT_PRESENT_VENDOR_ID 0xffff

void initialize_device(unsigned short vendor_id, unsigned short device_id) {
    if (vendor_id == DEVICE_NOT_PRESENT_VENDOR_ID) {
        return;
    }

    void* initialization_function_address = get_initialization_function_address(vendor_id, device_id);
    call_function_at_address(initialization_function_address);
}

#define NUM_DEVICE_SLOTS 5

void initialize_all_present_devices() {
    for (char d = 0; d < NUM_DEVICE_SLOTS; d++) {
        unsigned short vendor_id = read_vendor_id(d);
        unsigned short device_id = read_device_id(d);
        initialize_device(vendor_id, device_id);
    }
}

void initialize_pci() {
    initialize_device_function_address_table();

    initialize_all_present_devices();
}
