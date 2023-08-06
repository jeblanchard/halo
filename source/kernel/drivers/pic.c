#include "../utils/low_level.h"
#include "../idt.h"
#include "screen.h"
#include "../utils/standard.h"
#include "../utils/errors.h"

#define PRIMARY_PIC_COMMAND_REG 0x20
#define SECONDARY_PIC_COMMAND_REG 0xa0

#define NUMBER_OF_IRQS 16

static bool irq_in_use_table[NUMBER_OF_IRQS];

void initialize_irq_in_use_table() {
    for (int i = 0; i < NUMBER_OF_IRQS; i++) {
        irq_in_use_table[i] = false;
    }
}

bool irq_is_already_taken(unsigned char irq_num) {
    return irq_in_use_table[irq_num];
}

void install_irq(unsigned short irq_num,
                 unsigned int handler_entry_address) {

    print_int_ln(irq_in_use_table[0]);
    print_int_ln(irq_num);

    if (irq_is_already_taken(irq_num)) {
        char err_msg[] = "IRQ number is already taken.";
        halt_and_display_error_msg(err_msg);
    }

    unsigned char ir_num = LAST_RESERVED_IR_NUM + 1 + irq_num;
    unsigned char flags = 0x8e;
    unsigned short segment_sel = 0x8;
    install_ir(ir_num, flags, segment_sel, handler_entry_address);

    irq_in_use_table[irq_num] = true;
}

// Sends a command to PIC 0 or 1
void pic_send_command(unsigned char cmd, unsigned char pic_num) {
    unsigned char reg;
    if (pic_num == 0) {
        reg = PRIMARY_PIC_COMMAND_REG;
    } else {
        reg = SECONDARY_PIC_COMMAND_REG;
    }

	port_byte_out(reg, cmd);
}

#define PRIMARY_PIC_STATUS_REG    0x20
#define SECONDARY_PIC_STATUS_REG  0xa0

// Reads from the status register of PIC
// [pic_num]
unsigned char read_pic_status(char pic_num) {
    unsigned char reg;
    if (pic_num == 0) {
        reg = PRIMARY_PIC_STATUS_REG;
    } else {
        reg = SECONDARY_PIC_STATUS_REG;
    }

    unsigned char status = port_byte_in(reg);
    return status;
}

// Read Interrupt Request Register command
#define READ_IRR_CMD 0x0a

unsigned char read_pic_irr(char pic_num) {
    pic_send_command(READ_IRR_CMD, pic_num);
    unsigned char irr = read_pic_status(pic_num);
    return irr;
}

// Read In-Service Register command
#define READ_ISR_CMD 0x0b

unsigned char read_pic_isr(char pic_num) {
    pic_send_command(READ_ISR_CMD, pic_num);
    unsigned char isr = read_pic_status(pic_num);
    return isr;
}

char pic_isr_is_clear(char pic_num) {
    unsigned char isr = read_pic_isr(pic_num);

    if (isr == 0) {
        return true;
    }

    return false;
}

#define PRIMARY_PIC_INT_MASK_REG     0x21
#define SECONDARY_PIC_INT_MASK_REG   0xa1

// Reads from the data register of PIC [pic_num]
unsigned char read_pic_imr(unsigned char pic_num) {

    unsigned char reg;
    if (pic_num == 0) {
        reg = PRIMARY_PIC_INT_MASK_REG;
    } else {
        reg = SECONDARY_PIC_INT_MASK_REG;
    }

    unsigned char mask = port_byte_in(reg);
    return mask;
}

// Sends data to PIC 0 or 1
void pic_send_data(unsigned char mask, unsigned char pic_num) {

    unsigned char reg;
    if (pic_num == 0) {
        reg = PRIMARY_PIC_INT_MASK_REG;
    } else {
        reg = SECONDARY_PIC_INT_MASK_REG;
    }

	port_byte_out(reg, mask);
}

// Sends ICW 1 to PIC 0 and 1.
//
// icw1:
//   Bit 0: If set(1), the PIC expects to receive IC4
//          during initialization.
//   Bit 1: If set(1), only one PIC in system. If cleared,
//          PIC is cascaded with slave PICs, and ICW 3
//          must be sent to controller.
//   Bit 2: If set (1), CALL address interval is 4, else 8.
//          This is usually ignored by x86, and is default to 0.
//   Bit 3: If set (1), operate in Level Triggered mode.
//          If not set (0), operate in Edge Triggered Mode.
//          By default, we are in Edge Triggered mode
//          so set to 0.
//   Bit 4: Initialization bit. Set to 1 if PIC is to be
//          initialized.
//   Bits 5-7: On x86 Architecture must be 0.
void send_icw1(unsigned char icw1) {
    pic_send_command(icw1, 0);
    pic_send_command(icw1, 1);
}

// Sends ICW 2 to PIC [pic_num]
//
// icw2:
//   Specifies the IR number to associate
//   with the beginning of a PIC's IRQs.
void send_icw2(unsigned char icw2, char pic_num) {
    pic_send_data(icw2, pic_num);
}

// Sends ICW 3 to primary PIC
//
// icw3:
//   Specifies which IRQ line is
//   connected to the slave PIC.
//   Each bit represents an IRQ
//   line.
void send_icw3_primary_pic(unsigned char icw3) {
    pic_send_data(icw3, 0);
}

// Sends ICW 3 to secondary PIC
//
// icw3:
//  Bits 0-2: IRQ line the master PIC uses to
//            connect (in normal binary notation)
//  Bits 3-7: Reserved, must be 0
void send_icw3_secondary_pic(unsigned char icw3) {
    pic_send_data(icw3, 1);
}

// Sends ICW 4 to PIC [pic_num]
//
// icw4:
//   Bit 0: If set (1), it is in 80x86 mode. Cleared
//          if MCS-80/86 mode.
//   Bit 1: (AEOI) If set, on the last interrupt
//          acknowledge pulse, controller automatically
//          performs End of Interrupt (EOI) operation.
//   Bit 2: Only use if BUF is set. If set,
//          selects buffer master. Cleared
//          if buffer slave.
//   Bit 3: (BUF) If set, controller operates
//          in buffered mode
//   Bit 4: Special Fully Nested Mode,
//          not supported by the x86
//          architecture family
//   Bits 5-7: Reserved, must be 0
void send_icw4(unsigned char icw4, unsigned char pic_num) {
    pic_send_data(icw4, pic_num);
}

/* Sends OCW 1 to PIC [pic_num]
 *
 * OCW 1 sets and clears the mask bits
 * in the Interrupt Mask Register (IMR).
 *
 * Bits 0-7:
 *   1: The channel is masked (inhibited).
 *   0: The channel is enabled. */
void send_ocw1(unsigned char ocw1, unsigned char pic_num) {
    unsigned char reg;
    if (pic_num == 0) {
        reg = PRIMARY_PIC_COMMAND_REG;
    } else {
        reg = SECONDARY_PIC_COMMAND_REG;
    }

    pic_send_command(reg, ocw1);
}

// Sends OCW 2 to PIC [pic_num]
//
// ocw2:
//   Bits 0-2: Interrupt level upon which the controller
//             must react
//   Bits 3-4: Reserved, must be 0
//   Bits 5-7: Commands
//     000: Rotate in Automatic EOI mode
//     001: Non specific EOI command
//     010: No operation
//     011: Specific EOI command
//     101: Rotate on non specific EOI
//     110: Set priority command
//     111: Rotate on specific EOI
void send_ocw2(unsigned char ocw2, unsigned char pic_num) {
    unsigned char reg;
    if (pic_num == 0) {
        reg = PRIMARY_PIC_COMMAND_REG;
    } else {
        reg = SECONDARY_PIC_COMMAND_REG;
    }

    pic_send_command(reg, ocw2);
}

#define EOI_COMMAND 0x20

// Send End of Interrupt signal to
// PIC [pic_num]
void send_eoi(char pic_num) {
    send_ocw2(EOI_COMMAND, pic_num);
}

// Initializes PIC 0 and 1.
void initialize_pic() {

	unsigned char icw1 = 0x11;   // 0001 0001
	send_icw1(icw1);

	unsigned char icw2_pic0 = LAST_RESERVED_IR_NUM + 1;
	send_icw2(icw2_pic0, 0);

	unsigned char icw2_pic1 = LAST_RESERVED_IR_NUM + 9;
	send_icw2(icw2_pic1, 1);

	unsigned char primary_icw3 = 0x04;
	send_icw3_primary_pic(primary_icw3);

	unsigned char secondary_icw3 = 0x02;
	send_icw3_secondary_pic(secondary_icw3);

	unsigned char icw4 = 0x03;
	send_icw4(icw4, 0);
	send_icw4(icw4, 1);

	// Null out the data registers
	pic_send_data(0, 0);
	pic_send_data(0, 1);

	initialize_irq_in_use_table();
//	char init_msg[] = "At PIC init: ";
//	print(init_msg);
//	print_int_ln(irq_in_use_table[0]);
}
