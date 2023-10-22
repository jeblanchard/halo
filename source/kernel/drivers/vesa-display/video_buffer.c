#include "../../utils/io.h"
#include "video_buffer.h"

static unsigned char * VIDEO_ADDRESS = (unsigned char *) 0xb8000;

void set_byte_in_video_buffer(unsigned int offset, unsigned char val) {
    VIDEO_ADDRESS[offset] = val;
}

// Screen device I/O ports
#define SCREEN_CTRL_REG 0x3D4
#define SCREEN_DATA_REG 0x3D5

unsigned int get_cursor_offset() {

    // The device uses its control register as an index
    // to select its internal registers, of which we are
    // interested in:
    //   reg 14: which is the high byte of the cursor’s offset
    //   reg 15: which is the low byte of the cursor’s offset
    // Once the internal register has been selected, we may read or
    // write a byte on the data register.
    port_byte_out(SCREEN_CTRL_REG, 14);
    unsigned int offset = port_byte_in(SCREEN_DATA_REG) << 8;
    port_byte_out(SCREEN_CTRL_REG, 15);
    offset += port_byte_in(SCREEN_DATA_REG);

    // Since the cursor offset reported by the VGA hardware is the
    // number of characters, we multiply by two to convert it to
    // a character cell offset.
    return offset * CONFIG_BYTE_PERIOD;
}

void set_cursor(unsigned int cell_offset) {

    // Convert from cell offset to char offset.
    cell_offset /= 2;

    int high_byte = cell_offset >> 8;

    int low_byte_tmp = cell_offset << 24;
    int low_byte = low_byte_tmp >> 24;

    // The device uses its control register as an index
    // to select its internal registers, of which we are
    // interested in:
    //   reg 14: which is the high byte of the cursor’s offset
    //   reg 15: which is the low byte of the cursor’s offset
    // Once the internal register has been selected, we may read or
    // write a byte on the data register.
    port_byte_out(SCREEN_CTRL_REG, 14);
    port_byte_out(SCREEN_DATA_REG, high_byte);

    port_byte_out(SCREEN_CTRL_REG, 15);
    port_byte_out(SCREEN_DATA_REG, low_byte);
}

unsigned int get_screen_offset(unsigned int col, unsigned int row) {
    return (row * NUM_COLS + col) * CONFIG_BYTE_PERIOD;
}
