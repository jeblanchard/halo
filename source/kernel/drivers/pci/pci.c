#include "../utils/low_level.h"
#include "../utils/standard.h"
#include "../utils/errors.h"
#include "../dp8390-nic/functions.h"

void initialize_pci() {
    initialize_device_function_address_table();

    initialize_all_present_devices();
}

#define NUM_DEVICE_SLOTS 5
#define VENDOR_ID_CONFIG_SPACE_OFFSET 0x0

unsigned short read_vendor_id(unsigned char device_slot) {
    return pci_config_read_register(0, d, 0, VENDOR_ID_CONFIG_SPACE_OFFSET);
}

struct device_initializer {
    unsigned short vendor_id;
    unsigned short device_id;

    void* initialization_function_address;
};

#define KNOWN_NUMBER_OF_PCI_DEVICES 1
static struct device_initializer all_device_initializers[KNOWN_NUMBER_OF_PCI_DEVICES]

void initialize_device_function_address_table() {
    dp8390_initializer = all_device_initializers[0];
    dp8390_initializer.vendor_id = DP8390_VENDOR_ID;
    dp8390_initializer.device_id = DP8390_DEVICE_ID
    dp8390_initializer.initialization_function_address = initialize_dp8390;
}

void* get_initialization_address(unsigned short vendor_id, unsigned short device_id) {
    for (int i = 0; i < KNOWN_NUMBER_OF_PCI_DEVICES; i++) {
        struct device_initializer d = all_device_initializers[i];
        if (d.vendor_id == vendor_id && d.device_id == device_id) {
            return d.initialization_function_address;
        }
    }

    char msg[] = "Could not find PCI device.";
    halt_and_display_error_msg(msg);

    // We should never get here.
    return;
}

#define DEVICE_NOT_PRESENT_VENDOR_ID 0xffff

void initialize_device(unsigned short vendor_id, unsigned short device_id) {
    if (vendor_id == DEVICE_NOT_PRESENT_VENDOR_ID) {
        return;
    }

    void* initialization_function_address = get_initialization_address(vendor_id, device_id);
    call_function_at_address(initialization_function_address);
}

void initialize_all_present_devices() {
    for (char d = 0; d < NUM_DEVICE_SLOTS; d++) {
        unsigned short vendor_id = read_vendor_id(d);
        initialize_device(vendor_id);
    }
}

#define CONFIG_ADDRESS 0xcf8
#define CONFIG_DATA 0xcfc

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

//#pragma pack(push, 1)
//struct base_address_register {
//
//	// Bits 0-15, bits 0-15 of segment limit
//	unsigned char
//
//	// Bits 16-39, bits 0-23 of segment base
//	// address.
//	// Defines the location of byte 0 of the
//	// segment within the 4 GB linear
//	// address space.
//	unsigned short  base_low;
//	unsigned char   base_mid;
//
//    // Bits 40-43, type field
//    // Bit 44, descriptor type flag
//    // Bits 45-46, descriptor privilege level field
//    // Bit 47, segment-present flag
//	unsigned char access;
//
//    // Bits 48-51, bits 16-19 of segment limit
//    // Bit 52, available for use by system software
//    // Bit 53, 64-bit code segment flag
//    // Bit 54, D/B flag
//    // Bit 55, granularity flag
//	unsigned char granularity;
//
//    // Bits 56-63, bits 24-31 of the segment base
//    // address
//	unsigned char base_hi;
//};
//#pragma pack(pop)
