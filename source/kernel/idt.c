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

// A struct that represents the contents of the IDTR register
#pragma pack(push,1)
struct idtr {

    // Location of the last valid byte in the IDT.
    // Because IDT entries are always eight bytes long,
    // the limit should always be one less than an
    // integral multiple of eight (that is, 8N – 1) - where
    // N is equal to the number of interrupt descriptors
    // stored in the IDT.
	unsigned short		limit;

	// Base address of IDT
	unsigned int		idt_base_address;
};
#pragma pack(pop)

// Installs our IDTR variable into processor's IDTR register
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

    asm ( "lidt %0" : : "m"(idtr) );  // let the compiler choose an addressing mode
}

//void get_stored_idtr() {
//    __asm__ volatile ("lidt %0"
//         : /* No outputs. */
//         : "m" (idtr_addr));
//}

// Our Interrupt Descriptor Table
//struct idt_descriptor idt[MAX_IR_NUM + 1];
struct idt_descriptor idt[5];

// Nulls out the area of memory occupied by the IDT.
void clear_idt() {
    void* idt_base_address = &idt[0];
    int num_bytes_to_null_out = sizeof(idt);
    set_memory(idt_base_address, 15, num_bytes_to_null_out);
}

void enable_hardware_interrupts() {
    asm volatile ("sti");
}

void return_from_int_handler() {
    send_eoi();
    asm volatile ("iret");
}

// Default handler to catch unhandled system interrupts
void default_handler() {

    // Print information about the interrupt
    // that was triggered
    char message[] = "Default handler was triggered. System halted.";
    print(message);

    // Halt the system
	for(;;);
}

void populate_idtr() {
    idtr_.limit = (unsigned short) sizeof(idt) - 1;
    idtr_.idt_base_address = (unsigned int) &idt;
}

// Initialize IDT
void initialize_idt() {
	clear_idt();
    char clear_idt_msg[] = "Just cleared IDT.\n";
    print(clear_idt_msg);

    char test_num_msg[] = "Test number: ";
    print(test_num_msg);

    unsigned int test_num = 0xfffffff;
    print_int_ln_hex(test_num);

    char int_size_msg[] = "Size of unsigned int: ";
    print(int_size_msg);

    int int_size = sizeof(unsigned int);
    print_int_ln(int_size);

    char idt_desc_size_msg[] = "Size of IDT descriptor: ";
    print(idt_desc_size_msg);

    int idt_desc_size = sizeof(struct idt_descriptor);
    print_int_ln(idt_desc_size);

	// Set each entry to the default handler
	unsigned char flags = 0x8e;   // 1000 1110
	unsigned short segment_sel = 0x8; // 0000 0000 0000 1000
	unsigned int default_handler_address = (unsigned int) default_handler;
	for (unsigned short ir_num = 0; ir_num <= MAX_IR_NUM; ir_num++) {
        install_ir(ir_num,
                   flags,
                   segment_sel,
                   default_handler_address);
	}

    char def_hand_addr_msg[] = "Default handler address: ";
    print(def_hand_addr_msg);
    print_int_ln_hex(default_handler_address);

    char def_hand_addr_size_msg[] = "Size of default handler address: ";
    print(def_hand_addr_size_msg);
    unsigned int def_hand_size = (unsigned int) sizeof(default_handler_address);
    print_int_ln(def_hand_size);

    char idt_low_elem_msg[] = "Default handler address low, index 2 of IDT: ";
    print(idt_low_elem_msg);

    int idt_2_hand_addr_low = (int) idt[2].handler_address_low;
    print_int_ln_hex(idt_2_hand_addr_low);

    char idt_high_elem_msg[] = "Default handler address high, index 2 of IDT: ";
    print(idt_high_elem_msg);

    int idt_2_hand_addr_high = (int) idt[2].handler_address_high;
    print_int_ln_hex(idt_2_hand_addr_high);
//
//
//    char message[] = "Filled IDT with blanks.\n";
//    print(message);
//
//    char idt_size_msg[] = "Size of IDT: ";
//    print(idt_size_msg);
//
//    int idt_size = sizeof(idt);
//    print_int_ln(idt_size);
//
    char idt_addr_msg[] = "Address of IDT: ";
    print(idt_addr_msg);

    int idt_addr = (int) &idt;
    print_int_ln_hex(idt_addr);

    populate_idtr();

    char idtr_idt_addr_msg[] = "Address of IDT in IDTR struct: ";
    print(idtr_idt_addr_msg);

    int idtr_idt_addr = (int) idtr_.idt_base_address;
    print_int_ln_hex(idtr_idt_addr);

    char idt_elem_size_msg[] = "Size of IDT element (2): ";
    print(idt_elem_size_msg);

    int idt_elem_size = sizeof(idt[2]);
    print_int_ln(idt_elem_size);

    char idtr_limit_msg[] = "Limit of IDT in IDTR struct: ";
    print(idtr_limit_msg);

    int idtr_limit = (int) idtr_.limit;
    print_int_ln(idtr_limit);
//
//    char idtr_size_msg[] = "Size of IDTR variable: ";
//    print(idtr_size_msg);
//
//    int idtr_size = sizeof(idtr_);
//    print_int_ln(idtr_size);
//
//    char idtr_struct_size_msg[] = "Size of IDTR struct: ";
//    print(idtr_struct_size_msg);
//
//    int idtr_struct_size = sizeof(struct idtr);
//    print_int_ln(idtr_struct_size);
//
//    unsigned int idtr_addr = (unsigned int) &idtr_;
//
//    char idtr_addr_msg[] = "Address of IDTR struct: ";
//    print(idtr_addr_msg);
//    print_int_ln_hex(idtr_addr);

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
void gen_interrupt(unsigned char int_num) {
    asm ("int $11"
         : /* No outputs. */
         : [int_num] "m" (int_num));
}
