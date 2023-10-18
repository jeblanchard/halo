// Initialize the PIC.
void initialize_pic();

// Send End of Interrupt signal to
// PIC [pic_num]
void send_eoi(char pic_num);

unsigned char read_pic_status(char pic_num);

unsigned char read_pic_imr(unsigned char pic_num);

void pic_send_data(unsigned char mask, unsigned char pic_num);

unsigned char read_pic_irr(char pic_num);

unsigned char read_pic_isr(char pic_num);

char pic_isr_is_clear(char pic_num);

void install_irq(unsigned short irq_num, void* handler_entry_address);

extern unsigned short PRIMARY_PIC_INT_MASK_REG;

extern unsigned short SECONDARY_PIC_INT_MASK_REG;

extern unsigned short get_primary_pic_int_mask_reg();

unsigned short get_secondary_pic_int_mask_reg();

extern unsigned short PRIMARY_PIC_COMMAND_REG;

extern unsigned short SECONDARY_PIC_COMMAND_REG;
