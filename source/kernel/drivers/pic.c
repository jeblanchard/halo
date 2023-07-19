#include "pic.h"
#include "../utils/low_level.h"
#include "../idt.h"
#include "screen.h"

// Primary PIC Interrupt Mask Register and
// Data Register
#define PRIMARY_PIC_DATA_REG 0x21;

// Secondary (Slave) PIC Interrupt Mask and
// Data Register
#define SECONDARY_PIC_DATA_REG 0xa1;

// Reads data from PIC [pic_num]
unsigned char pic_read_data(unsigned char pic_num) {

    unsigned char reg;
    if (pic_num == 0) {
        reg = PRIMARY_PIC_DATA_REG;
    } else {
        reg = SECONDARY_PIC_DATA_REG;
    }

    unsigned char data = port_byte_in(reg);
    return data;
}

// Sends data to PIC 0 or 1
void pic_send_data(unsigned char data, unsigned char pic_num) {

    unsigned char reg;
    if (pic_num == 0) {
        reg = PRIMARY_PIC_DATA_REG;
    } else {
        reg = SECONDARY_PIC_DATA_REG;
    }

	port_byte_out(reg, data);
}

// Primary PIC Command and Status Register
#define PRIMARY_PIC_COMMAND_REG 0x20

// Secondary (Slave) PIC Command and Status
// Register
#define SECONDARY_PIC_COMMAND_REG 0xa0

// Sends a command to PIC 0 or 1
void pic_send_command(unsigned char cmd, unsigned char pic_num) {

	if (pic_num > 1)
		return;

    unsigned char reg;
    if (pic_num == 0) {
        reg = PRIMARY_PIC_COMMAND_REG;
    } else {
        reg = SECONDARY_PIC_COMMAND_REG;
    }

	port_byte_out(reg, cmd);
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

// Sends ICW to PIC [pic_num]
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
//   Bits 4: Special Fully Nested Mode,
//           not supported by the x86
//           architecture family
//   Bits 5-7: Reserved, must be 0
void send_icw4(unsigned char icw4, unsigned char pic_num) {
    pic_send_data(icw4, pic_num);
}

// Sends OCW 2 to PIC [pic_num]
//
// ocw2:
//   Bits 0-2: Interrupt level upon which the controller
//             must react
//   Bits 3-4: Reserved, must be 0
//   Bits 5-7: Commands
//     000: Rotate in Automatic EOI mode
//     100: Non specific EOI command
//     010: No operation
//     110: Specific EOI command
//     101: Rotate on non specific EOI
//     011: Set priority command
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

	unsigned char icw4 = 0x01;
	send_icw4(icw4, 0);
	send_icw4(icw4, 1);

    char message[] = "PIC finished initializing.\n";
    print(message);
}
