#include "./drivers/screen.h"
#include "idt.h"
#include "./utils/memory.h"
#include "./drivers/pic.h"
#include "./utils/string.h"

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

// Make C extern declarations of the ISR entry points
extern void default_handler_entry();

// Define an ISR stub that makes a call to a C function
__asm__ (".global _default_handler_entry\n"
         "_default_handler_entry:\n\t"
         "cld\n\t"                    // Set direction flag forward for C functions
         "pusha\n\t"                  // Save all the registers
         "call _default_handler\n\t"
         "popa\n\t"                   // Restore all the registers
         "iret");

// Default handler to catch unhandled system interrupts
void default_handler() {
    char message[] = "Default handler was triggered.";
    print_ln(message);

    for (;;) {};
}

// Initialize IDT
void initialize_idt() {
	clear_idt();
    char clear_idt_msg[] = "Just cleared IDT.\n";
    print(clear_idt_msg);

	// Set each entry to the default handler
	unsigned char flags = 0x8e;       // 1000 1110
	unsigned short segment_sel = 0x8; // 0000 0000 0000 1000
	unsigned int default_handler_address = (unsigned int) default_handler_entry;
	for (unsigned short ir_num = 0; ir_num <= MAX_IR_NUM; ir_num++) {
        install_ir(ir_num,
                   flags,
                   segment_sel,
                   default_handler_address);
	}

    unsigned short limit = (unsigned short) sizeof(idt) - 1;
    load_idt(&idt, limit);

    char load_idt_msg[] = "Loaded IDT.\n";
    print(load_idt_msg);
}

void install_ir(unsigned char ir_num,
                unsigned char flags,
                unsigned short segment_sel,
                unsigned int handler_address) {

	// Store handler address in IDT
	unsigned short handler_address_low = (unsigned short) handler_address & 0xffff;
	unsigned short handler_address_high = (unsigned short) (handler_address >> 16) & 0xffff;

	idt[ir_num].handler_address_low = handler_address_low;
	idt[ir_num].handler_address_high = handler_address_high;
	idt[ir_num].reserved = 0;
	idt[ir_num].flags = flags;
	idt[ir_num].segment_sel = segment_sel;
}

// Generate interrupt call
void gen_interrupt() {
    asm volatile ("int $32" : : );
}
