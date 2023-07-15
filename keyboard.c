#include "../utils/low_level.h"
#include "keyboard.h"
#include "screen.h"

// This is a scancode table used to represent a
// standard US keyboard.
unsigned char keyboard_us[128] = {

   0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	/* 9 */
  '9', '0', '-', '=', '\b',	/* Backspace */
  '\t',			/* Tab */
  'q', 'w', 'e', 'r',	/* 19 */
  't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	/* Enter key */
    0,			/* 29   - Control */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
 '\'', '`',   0,		/* Left shift */
 '\\', 'z', 'x', 'c', 'v', 'b', 'n',			/* 49 */
  'm', ',', '.', '/',   0,				/* Right shift */
  '*',
    0,	/* Alt */
  ' ',	/* Space bar */
    0,	/* Caps lock */
    0,	/* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,	/* < ... F10 */
    0,	/* 69 - Num lock*/
    0,	/* Scroll Lock */
    0,	/* Home key */
    0,	/* Up Arrow */
    0,	/* Page Up */
  '-',
    0,	/* Left Arrow */
    0,
    0,	/* Right Arrow */
  '+',
    0,	/* 79 - End key*/
    0,	/* Down Arrow */
    0,	/* Page Down */
    0,	/* Insert Key */
    0,	/* Delete Key */
    0,   0,   0,
    0,	/* F11 Key */
    0,	/* F12 Key */
    0,	/* All other keys are undefined */
};

// Prepares driver for use
void keyboard_install(int irq) {

	// Install our interrupt handler (IRQ 1 uses interrupt 33)
//	set_vect(irq, keyboard_irq_handler);

	// Assume Basic Assurance test (BAT) test is good
	// (otherwise we likely would have seen something
	// during boot up).
	bool keyboard_bat_test_result = true;

	// Set lock keys and LED lights
	keyboard_set_leds(false, false, false);
}

// Handles the keyboard interrupt
void keyboard_irq_handler() {
    unsigned char scancode;

    /* Read from the keyboard's data buffer */
    scancode = port_byte_in(KEYBOARD_DATA_REG);

    /* If the top bit of the byte we read from the keyboard is
    *  set, that means that a key has just been released */
    if (scancode & 0x80) {

        // Will later include logic to handle SHIFT, ALT, etc.
        // keys getting released.
    } else {

        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts. */

        /* Just to show you how this works, we simply translate
        *  the keyboard scancode into an ASCII value, and then
        *  display it to the screen. You can get creative and
        *  use some flags to see if a shift is pressed and use a
        *  different layout, or you can add another 128 entries
        *  to the above layout to correspond to 'shift' being
        *  held. If shift is held using the larger lookup table,
        *  you would add 128 to the scancode when you look for it */

        char key = keyboard_us[scancode];
        print_char(key, -1, -1, 0);
    }
}

enum KYBRD_CTRL_STATS_MASK {
	KYBRD_CTRL_STATS_MASK_OUT_BUF	=	1,		    // 00000001
	KYBRD_CTRL_STATS_MASK_IN_BUF	=	2,		    // 00000010
	KYBRD_CTRL_STATS_MASK_SYSTEM	=	4,		    // 00000100
	KYBRD_CTRL_STATS_MASK_CMD_DATA	=	8,		    // 00001000
	KYBRD_CTRL_STATS_MASK_LOCKED	=	0x10,		// 00010000
	KYBRD_CTRL_STATS_MASK_AUX_BUF	=	0x20,		// 00100000
	KYBRD_CTRL_STATS_MASK_TIMEOUT	=	0x40,		// 01000000
	KYBRD_CTRL_STATS_MASK_PARITY	=	0x80		// 10000000
};

// Read status from keyboard controller
unsigned char kybrd_ctrl_read_status() {
	return port_byte_in(KYBRD_CTRL_STATS_REG);
}

// Send command byte to keyboard controller
void send_keyboard_ctrl_cmd(unsigned char cmd) {

	// Wait for keyboard controller input buffer to be clear
	while (1)
		if ( (kybrd_ctrl_read_status() & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
			break;

	port_byte_out(KYBRD_CTRL_CMD_REG, cmd);
}

// Read keyboard encoder buffer
unsigned char kybrd_enc_read_buf() {
	return port_byte_in(KYBRD_ENC_INPUT_BUF);
}

// Send command byte to keyboard encoder
void kybrd_enc_send_cmd(unsigned char cmd) {

	// Wait for keyboard controller input buffer to be clear
	while (1)
		if ( (kybrd_ctrl_read_status() & KYBRD_CTRL_STATS_MASK_IN_BUF) == 0)
			break;

	// Send command byte to kybrd encoder
	port_byte_out(KYBRD_ENC_CMD_REG, cmd);
}

// Sets LEDs
void set_keyboard_leds(bool num_lock, bool caps_lock, bool scroll) {

	unsigned char data = 0;

	// Set or clear the bit
	data = (scroll) ? (data | 1) : (data & 1);
	data = (num) ? (num | 2) : (num & 2);
	data = (caps) ? (num | 4) : (num & 4);

	//! send the command -- update keyboard Light Emetting Diods (LEDs)
	kybrd_enc_send_cmd (KYBRD_ENC_CMD_SET_LED);
	kybrd_enc_send_cmd (data);
}

// Enables the keyboard
void enable_keyboard() {
	send_keyboard_ctrl_cmd(KYBRD_CTRL_CMD_ENABLE);
	keyboard_enabled = true;
}

// Disables the keyboard
void disable_keyboard() {
	send_keyboard_ctrl_cmd(KYBRD_CTRL_CMD_DISABLE);
	keyboard_enabled = false;
}
