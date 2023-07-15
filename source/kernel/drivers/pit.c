#include "../utils/low_level.h"
#include "../idt.h"
#include "screen.h"
#include "pit.h"

// Global Tick count
unsigned int pit_ticks = 0;

void pit_irq_handler() {

	// increment tick count
	pit_ticks++;

    char message[] = "Handled PIT interrupt.";
    print(message);

    // Stall
    for (;;) {};

	return_from_int_handler();
}

// Send command to the PIC.
void pit_send_command(unsigned char cmd) {
	port_byte_out(PIT_COMMAND_REG, cmd);
}

// Send data to a counter.
void pit_send_data(unsigned short data, unsigned char counter) {

	unsigned char port;
	if (counter == 0) {
	    port = PIT_COUNTER_0_REG;
	} else if (counter == 1) {
	    port = PIT_COUNTER_1_REG;
	} else {
	    port = PIT_COUNTER_2_REG;
	}

	port_byte_out(port, data);
}

// Read data from a counter.
unsigned char pit_read_data(unsigned char counter) {

	unsigned char port;
	if (counter == 0) {
	    port = PIT_COUNTER_0_REG;
	} else if (counter == 1) {
	    port = PIT_COUNTER_1_REG;
	} else {
	    port = PIT_COUNTER_2_REG;
	}

	return port_byte_in(port);
}

void pit_set_counter(unsigned int freq, unsigned char ocw) {

	unsigned short divisor = 1193180 / freq;

	pit_send_command(ocw);

    unsigned char divisor_low = divisor & 0xff;
    unsigned char divisor_high = (divisor >> 8) & 0xff;

	// set frequency rate
	pit_send_data(divisor_low, 0);
	pit_send_data(divisor_high, 0);

	// reset tick count
	pit_ticks = 0;
}

// Initialize PIT
void initialize_pit() {
    unsigned char flags = 0x8e; // 1000 1110
    unsigned short segment_sel = 0x0008; // 0000 0000 0000 1000
    unsigned int handler_address = (unsigned int) &pit_irq_handler;

    unsigned char ir_num = LAST_RESERVED_IR_NUM + 1 + PIT_IRQ_NUM;

    install_ir(ir_num,
               flags,
               segment_sel,
               handler_address);

    unsigned char ocw = 0x37; // 0011 0111
    pit_set_counter(10, ocw);

    char message[] = "PIT initialized.\n";
    print(message);
}
