// Initialize IDT
void initialize_idt();

//// Adds an interrupt routine to our IDT
//void install_ir(unsigned char ir_num, unsigned int handler_entry_address);

void install_ir(unsigned char ir_num,
                unsigned char flags,
                unsigned short segment_sel,
                unsigned int handler_entry_address);

#define MAX_IR_NUM 255

#define LAST_RESERVED_IR_NUM 31

// Enables hardware interrupts
void enable_hardware_interrupts();

// Generate interrupt call
void gen_interrupt();
