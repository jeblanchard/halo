#include "./drivers/screen.h"
#include "./utils/memory.h"
#include "./drivers/pic.h"
#include "./utils/string.h"
#include "./utils/standard.h"
#include "./utils/errors.h"
#include "idt.h"

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

extern void default_handler_9_10_entry();

__asm__ (".global _default_handler_9_10_entry\n"
         "_default_handler_9_10_entry:\n\t"
         "cld\n\t"                    // Set direction flag forward for C functions
         "pusha\n\t"                  // Save all the registers
         "call _default_handler_9_10\n\t"
         "popa\n\t"                   // Restore all the registers
         "iret");

void default_handler_9_10() {
    char message[] = "Default handler 9 - 10 was triggered.";
    print_ln(message);

    for (;;) {};
}

extern void default_handler_11_20_entry();

__asm__ (".global _default_handler_11_20_entry\n"
         "_default_handler_11_20_entry:\n\t"
         "cld\n\t"                    // Set direction flag forward for C functions
         "pusha\n\t"                  // Save all the registers
         "call _default_handler_11_20\n\t"
         "popa\n\t"                   // Restore all the registers
         "iret");

void default_handler_11_20() {
    char message[] = "Default handler 11 - 20 was triggered.";
    print_ln(message);

    for (;;) {};
}

extern void default_handler_21_26_entry();

__asm__ (".global _default_handler_21_26_entry\n"
         "_default_handler_21_26_entry:\n\t"
         "cld\n\t"                    // Set direction flag forward for C functions
         "pusha\n\t"                  // Save all the registers
         "call _default_handler_21_26\n\t"
         "popa\n\t"                   // Restore all the registers
         "iret");

void default_handler_21_26() {
    char message[] = "Default handler 21 - 26 was triggered.";
    print_ln(message);

    for (;;) {};
}

extern void default_handler_27_29_entry();

__asm__ (".global _default_handler_27_29_entry\n"
         "_default_handler_27_29_entry:\n\t"
         "cld\n\t"                    // Set direction flag forward for C functions
         "pusha\n\t"                  // Save all the registers
         "call _default_handler_27_29\n\t"
         "popa\n\t"                   // Restore all the registers
         "iret");

void default_handler_27_29() {
    char message[] = "Default handler 27 - 29 was triggered.";
    print_ln(message);

    for (;;) {};
}

extern void default_handler_30_31_entry();

__asm__ (".global _default_handler_30_31_entry\n"
         "_default_handler_30_31_entry:\n\t"
         "cld\n\t"                    // Set direction flag forward for C functions
         "pusha\n\t"                  // Save all the registers
         "call _default_handler_30_31\n\t"
         "popa\n\t"                   // Restore all the registers
         "iret");

void default_handler_30_31() {
    char message[] = "Default handler 30 - 31 was triggered.";
    print_ln(message);

    for (;;) {};
}

extern void default_handler_32_entry();

__asm__ (".global _default_handler_32_entry\n"
         "_default_handler_32_entry:\n\t"
         "cld\n\t"                    // Set direction flag forward for C functions
         "pusha\n\t"                  // Save all the registers
         "call _default_handler_32\n\t"
         "popa\n\t"                   // Restore all the registers
         "iret");

void default_handler_32() {
    char message[] = "Default handler 32 was triggered.";
    print_ln(message);

    for (;;) {};
}

extern void ir_6_handler_entry();

__asm__ (".global _ir_6_handler_entry\n"
         "_ir_6_handler_entry:\n\t"
         "cld\n\t"
         "pusha\n\t"
         "call _handle_undefined_op_code\n\t"
         "popa\n\t"
         "iret");

void handle_undefined_op_code() {
    char msg[] = "Undefined Operation Code (OP Code) instruction.";
    print_ln(msg);

    for (;;) {};
}

extern void ir_7_handler_entry();

__asm__ (".global _ir_7_handler_entry\n"
         "_ir_7_handler_entry:\n\t"
         "cld\n\t"
         "pusha\n\t"
         "call _handle_no_coprocessor\n\t"
         "popa\n\t"
         "iret");

void handle_no_coprocessor() {
    char msg[] = "No coprocessor.";
    print_ln(msg);

    for (;;) {};
}

extern void ir_8_handler_entry();

__asm__ (".global _ir_8_handler_entry\n"
         "_ir_8_handler_entry:\n\t"
         "cld\n\t"
         "pusha\n\t"
         "call _handle_double_fault\n\t"
         "popa\n\t"
         "iret");

void handle_double_fault() {
    char msg[] = "Double fault.";
    print_ln(msg);

    for (;;) {};
}

extern void default_handler_0_5_entry();

__asm__ (".global _default_handler_0_5_entry\n"
         "_default_handler_0_5_entry:\n\t"
         "cld\n\t"                    // Set direction flag forward for C functions
         "pusha\n\t"                  // Save all the registers
         "call _default_handler_0_5\n\t"
         "popa\n\t"                   // Restore all the registers
         "iret");

void default_handler_0_5() {
    char message[] = "Default handler 0 - 5 was triggered.";
    print_ln(message);

    for (;;) {};
}

void divide_by_zero_handler() {
    char msg[] = "Divide by zero.";
    print_ln(msg);

    for (;;) {};
}

void debugger_single_step() {
    char msg[] = "Debugger single step.";
    print_ln(msg);

    for (;;) {};
}

void nmi_pin() {
    char msg[] = "Non Maskable Interrupt Pin.";
    print_ln(msg);

    for (;;) {};
}

void debugger_breakpoint() {
    char msg[] = "Debugger breakpoint.";
    print_ln(msg);

    for (;;) {};
}

void overflow() {
    char msg[] = "Overflow.";
    print_ln(msg);

    for (;;) {};
}

void bounds_check() {
    char msg[] = "Bounds check.";
    print_ln(msg);

    for (;;) {};
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
//void add_handler_to_idt(unsigned char ir_num,
//                        unsigned char flags,
//                        unsigned short segment_sel,
//                        unsigned int handler_address) {
//
//	unsigned short handler_address_low = (unsigned short) handler_address & 0xffff;
//	unsigned short handler_address_high = (unsigned short) (handler_address >> 16) & 0xffff;
//
//	idt[ir_num].handler_address_low = handler_address_low;
//	idt[ir_num].handler_address_high = handler_address_high;
//	idt[ir_num].reserved = 0;
//	idt[ir_num].flags = flags;
//	idt[ir_num].segment_sel = segment_sel;
//}

//static bool non_default_ir_table[MAX_IR_NUM + 1];

//#define DEFAULT_FLAGS 0x8e
//#define DEFAULT_SEGMENT_SEL 0x8

//void install_default_ir(unsigned char ir_num, unsigned int handler_address) {
//
//    unsigned char default_flags = 0x8e;
//    unsigned short default_segment_sel = 0x8;
//    add_handler_to_idt(ir_num, default_flags, default_segment_sel, handler_address);
//
//    non_default_ir_table[ir_num] = false;
//}

//// Initialize IDT
//void initialize_idt() {
//	clear_idt();
//
////    char before_init_msg[] = "Before initializing IDT.";
////    print_ln(before_init_msg);
////    for (;;) {};
//
//	unsigned int default_handler_0_5_address = (unsigned int) default_handler_0_5_entry;
//    for (unsigned short ir_num = 0; ir_num <= 5; ir_num++) {
//        if (ir_num == 3) {
//            install_default_ir(ir_num, default_handler_0_5_address);
//            char first_handler_msg[] = "After adding first default handler.";
//            print_ln(first_handler_msg);
//            for (;;) {};
//        }
//    }
//
//    char one_eigth_of_init_msg[] = "One eighth of initializing IDT.";
//    print_ln(one_eigth_of_init_msg);
//    for (;;) {};
//
//    unsigned int ir_6_handler_address = (unsigned int) ir_6_handler_entry;
//    install_default_ir(6, ir_6_handler_address);
//
//    unsigned int ir_7_handler_address = (unsigned int) ir_7_handler_entry;
//    install_default_ir(7, ir_7_handler_address);
//
//    char one_quarter_of_init_msg[] = "One quarter of initializing IDT.";
//    print_ln(one_quarter_of_init_msg);
//    for (;;) {};
//
//    unsigned int ir_8_handler_address = (unsigned int) ir_8_handler_entry;
//    install_default_ir(8, ir_8_handler_address);
//
//    unsigned int default_handler_9_10_address = (unsigned int) default_handler_9_10_entry;
//    for (unsigned short ir_num = 9; ir_num <= 10; ir_num++) {
//        install_default_ir(ir_num, default_handler_9_10_address);
//    }
//
//    unsigned int default_handler_11_20_address = (unsigned int) default_handler_11_20_entry;
//    for (unsigned short ir_num = 11; ir_num <= 20; ir_num++) {
//        install_default_ir(ir_num, default_handler_11_20_address);
//    }
//
//    char middle_of_init_msg[] = "Middle of initializing IDT.";
//    print_ln(middle_of_init_msg);
//    for (;;) {};
//
//    unsigned int default_handler_21_26_address = (unsigned int) default_handler_21_26_entry;
//    for (unsigned short ir_num = 21; ir_num <= 26; ir_num++) {
//        install_default_ir(ir_num, default_handler_21_26_address);
//    }
//
//    unsigned int default_handler_27_29_address = (unsigned int) default_handler_27_29_entry;
//    for (unsigned short ir_num = 27; ir_num <= 29; ir_num++) {
//        install_default_ir(ir_num, default_handler_27_29_address);
//    }
//
//    unsigned int default_handler_30_31_address = (unsigned int) default_handler_30_31_entry;
//    for (unsigned short ir_num = 30; ir_num <= 31; ir_num++) {
//        install_default_ir(ir_num, default_handler_30_31_address);
//    }
//
//    unsigned int default_handler_32_address = (unsigned int) default_handler_32_entry;
//        install_default_ir(32, default_handler_32_address);
//
//    unsigned int default_handler_address = (unsigned int) default_handler_entry;
//    for (unsigned short ir_num = 33; ir_num <= MAX_IR_NUM; ir_num++) {
//        install_default_ir(ir_num, default_handler_address);
//    }
//
//    char before_idt_msg[] = "Before loading IDT.";
//    print_ln(before_idt_msg);
//    for (;;) {};
//
//    unsigned short limit = (unsigned short) sizeof(idt) - 1;
//    load_idt(&idt, limit);
//
//    char after_idt_msg[] = "Loaded IDT.";
//    print_ln(after_idt_msg);
//    for (;;) {};
//}

// Initialize IDT
void initialize_idt() {
	clear_idt();

	// Set each entry to the default handler
	unsigned char flags = 0x8e;       // 1000 1110
	unsigned short segment_sel = 0x8; // 0000 0000 0000 1000
	unsigned int default_handler_0_5_address = (unsigned int) default_handler_0_5_entry;
    for (unsigned short ir_num = 0; ir_num <= 5; ir_num++) {
        install_ir(ir_num,
                   flags,
                   segment_sel,
                   default_handler_0_5_address);
    }

    unsigned int ir_6_handler_address = (unsigned int) ir_6_handler_entry;
    install_ir(6, flags, segment_sel, ir_6_handler_address);

    unsigned int ir_7_handler_address = (unsigned int) ir_7_handler_entry;
    install_ir(7, flags, segment_sel, ir_7_handler_address);

    unsigned int ir_8_handler_address = (unsigned int) ir_8_handler_entry;
    install_ir(8, flags, segment_sel, ir_8_handler_address);

    unsigned int default_handler_9_10_address = (unsigned int) default_handler_9_10_entry;
    for (unsigned short ir_num = 9; ir_num <= 10; ir_num++) {
        install_ir(ir_num,
                   flags,
                   segment_sel,
                   default_handler_9_10_address);
    }

    unsigned int default_handler_11_20_address = (unsigned int) default_handler_11_20_entry;
    for (unsigned short ir_num = 11; ir_num <= 20; ir_num++) {
        install_ir(ir_num,
                   flags,
                   segment_sel,
                   default_handler_11_20_address);
    }

    unsigned int default_handler_21_26_address = (unsigned int) default_handler_21_26_entry;
    for (unsigned short ir_num = 21; ir_num <= 26; ir_num++) {
        install_ir(ir_num,
                   flags,
                   segment_sel,
                   default_handler_21_26_address);
    }

    unsigned int default_handler_27_29_address = (unsigned int) default_handler_27_29_entry;
    for (unsigned short ir_num = 27; ir_num <= 29; ir_num++) {
        install_ir(ir_num,
                   flags,
                   segment_sel,
                   default_handler_27_29_address);
    }

    unsigned int default_handler_30_31_address = (unsigned int) default_handler_30_31_entry;
    for (unsigned short ir_num = 30; ir_num <= 31; ir_num++) {
        install_ir(ir_num,
                   flags,
                   segment_sel,
                   default_handler_30_31_address);
    }

    unsigned int default_handler_32_address = (unsigned int) default_handler_32_entry;
        install_ir(32,
                   flags,
                   segment_sel,
                   default_handler_32_address);

    unsigned int default_handler_address = (unsigned int) default_handler_entry;
    for (unsigned short ir_num = 33; ir_num <= MAX_IR_NUM; ir_num++) {
        install_ir(ir_num,
                   flags,
                   segment_sel,
                   default_handler_address);
    }

    unsigned short limit = (unsigned short) sizeof(idt) - 1;
    load_idt(&idt, limit);
}



//bool ir_num_is_already_taken(unsigned char ir_num) {
//    return non_default_ir_table[ir_num];
//}

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

//void install_ir(unsigned char ir_num, unsigned int handler_address) {
//
//    if (ir_num_is_already_taken(ir_num)) {
//        char err_msg[] = "IR number is already taken.";
//        halt_and_display_error_msg(err_msg);
//    }
//
//    add_handler_to_idt(ir_num, DEFAULT_FLAGS, DEFAULT_SEGMENT_SEL, handler_address);
//
//    non_default_ir_table[ir_num] = true;
//}

// Generate interrupt call
void gen_interrupt() {
    asm volatile ("int $33" : : );
}
