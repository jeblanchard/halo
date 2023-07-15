// Initialize IDT
void initialize_idt();

// Adds an interrupt routine to our IDT
//
// flags:
//  Bits 0-2, must be 0, 1, 1
//  Bit 3, size of gate (1 = 32 bits, 0 = 16
//  bits)
//  Bit 4, must be 0
//  Bits 5-6, descriptor privilege level
//  Bit 7, segment present flag
//
// segment_sel:
//  Bits 0-1, requested privilege level
//  Bit 2, table indicator flag (0 for GDT, 1 for LDT)
//  Bits 3-15, index in table
void install_ir(unsigned char ir_num,
                unsigned char flags,
                unsigned short segment_sel,
                unsigned int handler_address);

#define MAX_IR_NUM 255

#define LAST_RESERVED_IR_NUM 31

// Returns from an Interrupt handler
void return_from_int_handler();

// Enables hardware interrupts
void enable_hardware_interrupts();

// Generate interrupt call
void gen_interrupt(unsigned char int_num);
