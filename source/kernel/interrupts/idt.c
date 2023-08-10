#include "../drivers/screen.h"
#include "../utils/memory.h"
#include "../drivers/8259a-pic/pic.h"
#include "../utils/string.h"
#include "../utils/standard.h"
#include "../utils/errors.h"
#include "idt.h"
#include "predefined_interrupt_loading.h"
#include "../gdt.h"

#pragma pack(push,1)
struct idt_descriptor {

	// Bits 0-15, bits 0-15 of procedure offset
	unsigned short   handler_address_low;

	// Bits 16-31, segment selector for
	// destination code segment

	// Bits 0-1, requested privilege level
	// Bit 2, table indicator flag
	// Bits 3-15, index
	// something is wrong here
	unsigned short   segment_sel;

	// Bits 32-36, reserved
	// Bits 37-39, must be 0
	unsigned char    reserved;

	// Bits 40-42, must be 0, 1, 1
	// Bit 43, size of gate (1 = 32 bits, 0 = 16
	// bits)
	// Bit 44, must 0
	// Bits 45-46, descriptor privilege level
	// Bit 47, segment present flag
	unsigned char flags;

	// Bits 48-63, bits 16-31 of procedure offset
	unsigned short	handler_address_high;
};
#pragma pack(pop)

// Installs an IDTR into the processor's IDTR register.
static inline void load_idt(void* base, unsigned short size) {

    #pragma pack(push,1)
    struct {

        // Location of the last valid byte in the IDT.
        // Because IDT entries are always eight bytes long,
        // the limit should always be one less than an
        // integral multiple of eight (that is, 8N – 1) - where
        // N is equal to the number of interrupt descriptors
        // stored in the IDT.
        unsigned short  limit;

        // Base address of IDT.
        void*           base;
    } idtr = { size, base };
    #pragma pack(pop)

    asm ("lidt %0" : : "m"(idtr));
}

// Our Interrupt Descriptor Table
struct idt_descriptor idt[MAX_IR_NUM + 1];

// Nulls out the area of memory occupied by the IDT.
void clear_idt() {
    void* idt_base_address = &idt[0];
    int num_bytes_to_null_out = sizeof(idt);
    set_memory(idt_base_address, 15, num_bytes_to_null_out);
}

void enable_hardware_interrupts() {
    asm volatile ("sti");
}

/* flags:
 *  Bits 0-2, must be 0, 1, 1
 *  Bit 3, size of gate (1 = 32 bits, 0 = 16
 *  bits)
 *  Bit 4, must be 0
 *  Bits 5-6, descriptor privilege level
 *  Bit 7, segment present flag
 *
 * segment_sel:
 *  Bits 0-1, requested privilege level
 *  Bit 2, table indicator flag (0 for GDT, 1 for LDT)
 *  Bits 3-15, index in table */
void add_handler_to_idt(unsigned char ir_num,
                        unsigned char flags,
                        unsigned short segment_sel,
                        void* handler_address) {

    unsigned int handler_address_int = (unsigned int) handler_address;

	unsigned short handler_address_low = (unsigned short) handler_address_int & 0xffff;
	unsigned short handler_address_high = (unsigned short) (handler_address_int >> 16) & 0xffff;

	idt[ir_num].handler_address_low = handler_address_low;
	idt[ir_num].handler_address_high = handler_address_high;
	idt[ir_num].reserved = 0;
	idt[ir_num].flags = flags;
	idt[ir_num].segment_sel = segment_sel;
}

#define DEFAULT_FLAGS 0x8e
#define DEFAULT_SEG_SELECTOR 0x8

void install_ir(unsigned char ir_num, void* handler_address) {
    add_handler_to_idt(ir_num, DEFAULT_FLAGS, DEFAULT_SEG_SELECTOR, handler_address);
}

// Initialize IDT
void initialize_idt() {
	clear_idt();

    load_initial_handlers_to_idt();

    unsigned short limit = (unsigned short) sizeof(idt) - 1;
    load_idt(&idt, limit);
}

// Generate interrupt call
void gen_interrupt() {
    asm volatile ("int $33" : : );
}
