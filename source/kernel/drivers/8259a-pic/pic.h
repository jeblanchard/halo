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

void install_irq(unsigned short irq_num,
                 unsigned int handler_entry_address);
