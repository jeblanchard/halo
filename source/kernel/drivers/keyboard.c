#include "../utils/low_level.h"
#include "keyboard.h"
#include "screen.h"
#include "../idt.h"
#include "../utils/standard.h"
#include "pic.h"

#define KEYBOARD_CONTROLLER_STATUS_PORT 0x64

/* Read status from keyboard controller
 *
 * Bit 0: Output Buffer Status
 *   0: Output buffer empty, don't read yet
 *   1: Output buffer full, please read me
 * Bit 1: Input Buffer Status
 *   0: Input buffer empty, can be written
 *   1: Input buffer full, don't write yet
 * Bit 2: System flag
 *   0: Set after power on reset
 *   1: Set after successful completion of the
 *      keyboard controllers self-test -
 *      Basic Assurance Test (BAT)
 * Bit 3: Command Data
 *   0: Last write to input buffer was data (via port 0x60)
 *   1: Last write to input buffer was a command (via port 0x64)
 * Bit 4: Keyboard Locked
 *   0: Locked
 *   1: Not locked
 * Bit 5: Auxiliary Output buffer full
 *   PS/2 Systems:
 *     0: Determines if read from port 0x60 is valid. If valid
 *        0 = Keyboard data
 *     1: Mouse data, only if you can read from port 0x60
 *   AT Systems:
 *     0: OK flag
 *     1: Timeout on transmission from keyboard controller to keyboard.
 *        This may indicate no keyboard is present.
 * Bit 6: Timeout
 *   0: OK flag
 *   1: Timeout
 *   PS/2: General Timeout
 *   AT: Timeout on transmission from keyboard to keyboard controller.
 *       Possibly parity error, in which case both bits 6 and 7 are
 *       set.
 * Bit 7:
 *   0: OK flag, no error
 *   1: Parity error with last byte
 */
unsigned char read_keyboard_controller_status() {
	return port_byte_in(KEYBOARD_CONTROLLER_STATUS_PORT);
}

bool keyboard_controller_is_ready_for_cmd() {
    unsigned char input_buffer_full = 2;
    unsigned char is_input_buffer_full = read_keyboard_controller_status() & input_buffer_full;

    if (is_input_buffer_full) {
        return false;
    } else {
        return true;
    }
}

#define KEYBOARD_ENCODER_CMD_PORT 0x60

// Send command byte to keyboard encoder
void send_cmd_to_keyboard_encoder(unsigned char cmd) {

	// Wait for keyboard controller input buffer to be clear
	while (true) {
		if (keyboard_controller_is_ready_for_cmd()) {
		    break;
		}
    }

	// Send command byte to keyboard encoder
	port_byte_out(KEYBOARD_ENCODER_CMD_PORT, cmd);
}

#define SET_LED_CMD 0xed

// Sets LEDs
void set_keyboard_leds(bool num_lock, bool caps_lock, bool scroll) {

    /*
     * Bit 0: Scroll lock LED
     *   0: off
     *   1: on
     * Bit 1: Num lock LED
     *   0: off
     *   1: on
     * Bit 2: Caps lock LED
     *   0: off
     *   1: on
     */
	unsigned char led_options = 0;

	if (num_lock) {
	    led_options = led_options | 1;
	}

    if (caps_lock) {
        led_options = led_options | 2;
    }

    if (scroll) {
        led_options = led_options | 4;
    }

	// Send the command to update the keyboard's LEDs
	send_cmd_to_keyboard_encoder(SET_LED_CMD);
	send_cmd_to_keyboard_encoder(led_options);
}

// This is a scancode table used to represent a
// standard US keyboard.
//unsigned char keyboard_us[128] = {'a'};
unsigned char keyboard_us[128];

void fill_keys() {
    unsigned char keys[128] = {
         0,  27, '1', '2', '3', '4', '5', '6', '7', '8',	// 9
         '9', '0', '-', '=', '\b',	// Backspace
         '\t', // Tab
         'q', 'w', 'e', 'r', // 19
         't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',	// Enter key
         0,			      // 29 - Control
         'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';',	/* 39 */
         '\'', '`',   0,      // Left shift
         '\\', 'z', 'x', 'c', 'v', 'b', 'n',    // 49
         'm', ',', '.', '/',   0,				// Right shift
         '*',
         0,	// Alt
         ' ',	// Space bar
         0,	// Caps lock
         0,	// 59 - F1 key
         0,   0,   0,   0,   0,   0,   0,   0,
         0,	// F10
         0,	// 69 - Num lock
         0,	// Scroll Lock
         0,	// Home key
         0,	// Up Arrow
         0,	// Page Up
         '-',
         0,	// Left Arrow
         0,
         0,	// Right Arrow
         '+',
         0,	// 79 - End key
         0,	// Down Arrow
         0,	// Page Down
         0,	// Insert Key
         0,	// Delete Key
         0,   0,   0,
         0,	// F11 Key
         0,	// F12 Key
         0,	// All other keys are undefined
    };

    for (int i = 0; i < 128; i++) {
        keyboard_us[i] = keys[i];
    }
}

#define KEYBOARD_ENCODER_READ_BUFFER 0x60

// Read keyboard encoder buffer. May return an error code.
unsigned char read_keyboard_encoder_buf() {
	return port_byte_in(KEYBOARD_ENCODER_READ_BUFFER);
}

// Handles the keyboard interrupt
void keyboard_irq_handler() {
//    char got_to_msg[] = "Got to keyboard IRQ handler.\n";
//    print(got_to_msg);

    // Read from the keyboard's data buffer
    unsigned char scancode = read_keyboard_encoder_buf();

//    char read_buf_msg[] = "Read keyboard encoder buffer.\n";
//    print(read_buf_msg);

    // If the top bit of the byte we read from the keyboard is
    // set, that means that a key has just been released
    if (scancode & 0x80) {
//        char released_key_msg[] = "Key was just released.";
//        print_ln(released_key_msg);

        // Currently does not handle SHIFT, ALT, etc.
        // keys getting released.
    } else {

        /* Here, a key was just pressed. Please note that if you
        *  hold a key down, you will get repeated key press
        *  interrupts.
        *
        *  We simply translate the keyboard scancode into an ASCII
        *  value, and then display it to the screen. You can get
        *  creative and use some flags to see if a shift is
        *  pressed and use a different layout, or you can add
        *  another 128 entries to the above layout to correspond to
        *  'shift' being held. If shift is held using the larger
        *  lookup table, you would add 128 to the scancode when
        *  you look for it */

//        char pressed_key_msg[] = "Key was just pressed.";

//        unsigned char key = keyboard_us[scancode];
        unsigned char key = keyboard_us[scancode];
//        print_int(scancode);
//        print_int(key);
        print_char(key, -1, -1, 0);


//        char print_key_msg[] = "Printed key.";
//        print_ln(print_key_msg);
    }

//    send_eoi(0);

//    char sent_eoi_msg[] = "Sent EOI.";
//    print_ln(sent_eoi_msg);

//    char handler_finished_msg[] = "Keyboard handler finished.";
//    print_ln(handler_finished_msg);
}

extern void keyboard_handler_entry();

__asm__ (".global _keyboard_handler_entry\n"
         "_keyboard_handler_entry:\n\t"
         "cld\n\t"
         "pusha\n\t"
         "call _keyboard_irq_handler\n\t"
         "popa\n\t"
         "iret");

#define KEYBOARD_CONTROLLER_CMD_PORT 0x64

// Send command byte to keyboard controller
void send_cmd_to_keyboard_controller(unsigned char cmd) {

	// Wait for keyboard controller input buffer to be clear
	while (true) {
		if (keyboard_controller_is_ready_for_cmd()) {
			break;
        }
    }

	port_byte_out(KEYBOARD_CONTROLLER_CMD_PORT, cmd);
}

#define SELF_TEST_CMD 0xaa
#define BAT_PASSED_STATUS 0x55

// Runs Basic Assurance Test (BAT)
bool bat_was_successful() {
     char start_msg[] = "BAT starting.\n";
     print(start_msg);

    send_cmd_to_keyboard_controller(SELF_TEST_CMD);
    char cmd_sent_msg[] = "Sent BAT command.\n";
    print(cmd_sent_msg);

    unsigned char bat_status = read_keyboard_encoder_buf();

    if (bat_status == BAT_PASSED_STATUS) {
        return true;
    }

    return false;
}

#define ENABLE_KEYBOARD_CMD 0xf4

// Enables the keyboard
void enable_keyboard() {
	send_cmd_to_keyboard_encoder(ENABLE_KEYBOARD_CMD);
}

void initialize_keyboard() {
    fill_keys();

    char start_msg[] = "Starting keyboard initialization.\n";
    print(start_msg);

	// Install our interrupt handler
    unsigned char ir_num = LAST_RESERVED_IR_NUM + 1 + KEYBOARD_IRQ_NUM;
    unsigned char flags = 0x8e; // 1000 1110
    unsigned short segment_sel = 0x0008; // 0000 0000 0000 1000
    unsigned int handler_entry_address = (unsigned int) keyboard_handler_entry;
    install_ir(ir_num, flags, segment_sel, handler_entry_address);

    char ir_msg[] = "Installed keyboard IR.\n";
    print(ir_msg);

    if (bat_was_successful()) {
        // do nothing
    } else {
        char bat_fail_msg[] = "Keyboard BAT failed.\n";
        print(bat_fail_msg);
        for (;;) {};
    }

    char bat_msg[] = "BAT passed.\n";
    print(bat_msg);

	// Set lock keys and LED lights to starting value
	set_keyboard_leds(false, false, false);

	enable_keyboard();

	char led_msg[] = "LEDs set.\n";
    print(led_msg);
}

#define DISABLE_KEYBOARD_CMD 0xf5

// Disables the keyboard
void disable_keyboard() {
	send_cmd_to_keyboard_encoder(DISABLE_KEYBOARD_CMD);
}
