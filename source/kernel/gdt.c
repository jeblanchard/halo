#include "gdt.h"
#include "utils/memory.h"
#include "./drivers/screen.h"

// A GDT descriptor defines the properties of a specific
// memory block and its permissions.
#pragma pack(push,1)
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
	unsigned char access;

    // Bits 48-51, bits 16-19 of segment limit
    // Bit 52, available for use by system software
    // Bit 53, 64-bit code segment flag
    // Bit 54, D/B flag
    // Bit 55, granularity flag
	unsigned char granularity;

    // Bits 56-63, bits 24-31 of the segment base
    // address
	unsigned char base_hi;
};
#pragma pack(pop)

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
                        unsigned char access,
                        unsigned char granularity) {

	unsigned short segment_base_address_low = (unsigned short) segment_base_address & 0xffff;
	unsigned char segment_base_address_mid = (unsigned char) (segment_base_address >> 16) & 0xff;
	unsigned char segment_base_address_hi = (unsigned char) (segment_base_address >> 24) & 0xff;

	gdt[gdt_index].base_low = segment_base_address_low;
	gdt[gdt_index].base_mid = segment_base_address_mid;
	gdt[gdt_index].base_hi	= segment_base_address_hi;

	gdt[gdt_index].segment_limit_low = segment_limit_low;
	gdt[gdt_index].access = access;
	gdt[gdt_index].granularity = granularity;
}

// Installs an GDTR into the processor's GDTR register.
static inline void load_gdt(void* base, unsigned short size) {

    #pragma pack(push,1)
    struct {

        // Location of the last valid byte in the GDT.
        // Because GDT entries are always eight bytes long,
        // the limit should always be one less than an
        // integral multiple of eight (that is, 8N – 1) - where
        // N is equal to the number of segment descriptors
        // stored in the GDT.
        unsigned short  limit;

        // Base address of GDT.
        void*           base;
    } gdtr = { size, base };
    #pragma pack(pop)

    asm ("lgdt %0" : : "m"(gdtr));
}

void set_gdt_null_descriptor() {
    set_gdt_descriptor(0, 0, 0, 0, 0);
}

// Initialize our GDT
void initialize_gdt() {
    clear_gdt();

    set_gdt_null_descriptor();

    // Set default code descriptor
    unsigned char code_access = 0x9a;
    unsigned char code_granularity = 0xcf;
    set_gdt_descriptor(1, 0, 0xffff, code_access, code_granularity);

    // Set default data descriptor
    unsigned char data_access = 0x92;
    unsigned char data_granularity = 0xcf;
    set_gdt_descriptor(2, 0, 0xffff, data_access, data_granularity);

    unsigned short limit = (unsigned short) sizeof(gdt) - 1;
    load_gdt(&gdt, limit);
}
