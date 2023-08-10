void initialize_idt();

void install_ir(unsigned char ir_num, void* handler_entry_address);

#define MAX_IR_NUM 50

#define LAST_RESERVED_IR_NUM 31

void enable_hardware_interrupts();

void gen_interrupt();

void add_handler_to_idt(unsigned char ir_num, void* handler_address);
