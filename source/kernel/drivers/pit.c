#include "../utils/memory.h"
#include "../interrupts/idt.h"
#include "vesa_display.h"
#include "pit.h"
#include "../utils/string.h"
#include "8259a-pic/pic.h"
#include "../utils/standard.h"
#include "../utils/io.h"

// Global Tick count
static int pit_ticks = 0;

int get_tick_count() {
    return pit_ticks;
}

void handle_pit_irq() {
	pit_ticks++;
	update_clock();
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

    install_irq(PIT_IRQ_NUM, handle_pit_irq);

    unsigned char ocw = 0x36; // 0011 0110
    pit_set_counter(50, ocw);
}
