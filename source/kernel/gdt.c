#include "gdt.h"
#include "utils/memory.h"
#include "./drivers/screen.h"

// A GDT descriptor defines the properties of a specific
// memory block and its permissions.
struct gdt_descriptor {

	// Bits 0-15, bits 0-15 of segment limit
	unsigned short  segment_limit_low;

	// Bits 16-39, bits 0-23 of segment base
	// address.
	// Defines the location of byte 0 of the
	// segment within the 4 GB linear
	// address space.
	unsigned short  base_low;
	unsigned char   base_mid;

    // Bits 40-43, type field
    // Bit 44, descriptor type flag
    // Bits 45-46, descriptor privilege level field
    // Bit 47, segment-present flag
    // Bits 48-51, bits 16-19 of segment limit
    // Bit 52, available for use by system software
    // Bit 53, 64-bit code segment flag
    // Bit 54, D/B flag
    // Bit 55, granularity flag
	unsigned short flags_and_fields;

    // Bits 56-63, bits 24-31 of the segment base
    // address
	unsigned char base_hi;
} __attribute__ ((packed));

// A struct representing the contents of the GDTR register
struct gdtr {

    // Location of the last valid byte in the GDT.
    // Because GDT entries are always eight bytes long,
    // the limit should always be one less than an
    // integral multiple of eight (that is, 8N – 1) - where
    // N is equal to the number of segment descriptors
    // stored in the GDT.
	unsigned short		limit;

	// Base address of GDT
	unsigned int		gdt_base_address;
} __attribute__ ((packed));

// A variable representing the contents of our GDTR register
static struct gdtr gdtr_;

// Global Descriptor Table (GDT)
struct gdt_descriptor gdt[3];

void clear_gdt() {
    void* gdt_base_address = &gdt[0];
    int num_bytes_to_null_out = sizeof(gdt);
    set_memory(gdt_base_address, 0, num_bytes_to_null_out);
}

// Sets a descriptor in the GDT.
void set_gdt_descriptor(unsigned char gdt_index,
                        unsigned int segment_base_address,
                        unsigned short segment_limit_low,
                        unsigned short flags_and_fields) {

	unsigned short segment_base_address_low = (unsigned short) segment_base_address & 0xffff;
	unsigned char segment_base_address_mid = (unsigned char) (segment_base_address >> 16) & 0xff;
	unsigned char segment_base_address_hi = (unsigned char) (segment_base_address >> 24) & 0xff;

	gdt[gdt_index].base_low = segment_base_address_low;
	gdt[gdt_index].base_mid = segment_base_address_mid;
	gdt[gdt_index].base_hi	= segment_base_address_hi;

	gdt[gdt_index].segment_limit_low = segment_limit_low;
	gdt[gdt_index].flags_and_fields = flags_and_fields;
}

// Installs GDTR
void load_gdt(unsigned int gdtr_address) {
    __asm__ volatile ("lgdt %0"
         : /* No outputs. */
         : "m" (gdtr_address));
}

void set_gdt_null_descriptor() {
    set_gdt_descriptor(0, 0, 0, 0);
}

// 0000 0000 0000 0010
const unsigned short TYPE_FIELD_READ_WRITE = 0x0002;

// 0000 0000 0001 0000
const unsigned short DESC_TYPE_FLAG_CODE_DATA_SEG = 0x0010;

// 0000 0000 0000 0000
const unsigned short DESC_PRIV_LEVEL_0 = 0;

// 0000 0000 1000 0000
const unsigned short SEG_PRESENT_FLAG_YES = 0x0080;

// 0000 1111 0000 0000
const unsigned short MAX_SEGMENT_LIMIT = 0x0f00;

// 0000 0000 0000 0000
const unsigned short AVL_NOT_USED = 0;

// 0000 0000 0000 0000
const unsigned short BIT_64_MODE_FLAG_OFF = 0;

// 0100 0000 0000 0000
const unsigned short D_B_FLAG_32_BIT_MODE = 0x4000;

// 1000 0000 0000 0000
const unsigned short GRANULARITY_FLAG_4_KB_UNITS = 0x8000;

unsigned short get_default_code_data_desc_flags() {
    unsigned short default_code_data_desc_flags =
        TYPE_FIELD_READ_WRITE | DESC_TYPE_FLAG_CODE_DATA_SEG |
        DESC_PRIV_LEVEL_0 | SEG_PRESENT_FLAG_YES | MAX_SEGMENT_LIMIT |
        AVL_NOT_USED | BIT_64_MODE_FLAG_OFF | D_B_FLAG_32_BIT_MODE |
        GRANULARITY_FLAG_4_KB_UNITS;

    return default_code_data_desc_flags;
}

void populate_gdtr() {
    gdtr_.limit = 23;
    gdtr_.gdt_base_address = (unsigned int) &gdt;
//    gdtr_.gdt_base_address = 7;
}

// Initialize our GDT
void initialize_gdt() {
    clear_gdt();
    char clear_gdt_msg[] = "Just cleared GDT.\n";
    print(clear_gdt_msg);

    set_gdt_null_descriptor();

    // Set default code descriptor
    unsigned short code_seg_desc = 0x9acf; // 1001 1010 1100 1111
    set_gdt_descriptor(1, 0, 0xffff, code_seg_desc);

    // Set default data descriptor
    unsigned short data_seg_desc = 0x92cf; // 1001 0010 1100 1111
    set_gdt_descriptor(2, 0, 0xffff, data_seg_desc);

    populate_gdtr();

    // load GDT
    unsigned int gdtr_addr = (unsigned int) &gdtr_;
    load_gdt(gdtr_addr);

    char load_gdt_msg[] = "Loaded GDT.\n";
    print(load_gdt_msg);

    for (;;) {};
}
