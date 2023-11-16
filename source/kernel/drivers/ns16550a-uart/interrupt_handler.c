#include "../../utils/io.h"
#include "stdbool.h"
#include "../../utils/errors.h"

typedef enum {
    NONE = 1,
    RECEIVER_LINE_STATUS = 5,
    RECEIVED_DATA_AVAIL = 4,
    TRANSMITTER_HOLDING_REG_EMPTY = 2,
    MODEM_STATUS = 0
} interrupt_type;

interrupt_type get_type_of_interrupt() {
    extern unsigned short INTERRUPT_ID_REG;

    interrupt_type type = port_byte_in(INTERRUPT_ID_REG);

    return type;
}

#define OVERRUN_ERROR_BIT_MASK 0x02

bool overrun_error_occured(unsigned char line_status) {
    if (line_status & OVERRUN_ERROR_BIT_MASK) {
        return true;
    }

    return false;
}


void handle_overrun_error() {}

#define PARITY_ERROR_BIT_MASK 0x04

bool parity_error_occured(unsigned char line_status) {
    if (line_status & PARITY_ERROR_BIT_MASK) {
        return true;
    }

    return false;
}

void handle_parity_error() {}

#define FRAMING_ERROR_BIT_MASK 0x08

bool framing_error_occured(unsigned char line_status) {
    if (line_status & FRAMING_ERROR_BIT_MASK) {
        return true;
    }

    return false;
}

void handle_framing_error() {}

#define BREAK_INTERRUPT_BIT_MASK 0x10

bool break_interrupt_error_occured(unsigned char line_status) {
    if (line_status & BREAK_INTERRUPT_BIT_MASK) {
        return true;
    }

    return false;
}

void handle_break_interrupt() {}

#define TRANSMITTER_HOLDING_REG_EMPTY_BIT_MASK 0x20

bool transmitter_holding_register_is_empty(unsigned char line_status) {
    if (line_status & TRANSMITTER_HOLDING_REG_EMPTY_BIT_MASK) {
        return true;
    }

    return false;
}

void handle_transmitter_holding_register_empty() {}

#define TRANSMITTER_IS_EMPTY_BIT_MASK 0x40

bool transmitter_is_empty(unsigned char line_status) {
    if (line_status & TRANSMITTER_IS_EMPTY_BIT_MASK) {
        return true;
    }

    return false;
}

void handle_transmitter_empty() {}

bool data_is_ready(unsigned char line_status) {
    (void) line_status;
    return false;
}

void transfer_data_from_receive_buffer_to_host() {}

bool break_interrupt_occured(unsigned char line_status) {
    (void) line_status;
    return false;
}

void handle_receiver_line_status_interrupt() {
    extern unsigned short LINE_STATUS_REG;
    unsigned char line_status = port_byte_in(LINE_STATUS_REG);
    
    if (data_is_ready(line_status)) {
        transfer_data_from_receive_buffer_to_host();
    }

    if (overrun_error_occured(line_status)) {
        handle_overrun_error();
    }

    if (parity_error_occured(line_status)) {
        handle_parity_error();
    }

    if (framing_error_occured(line_status)) {
        handle_framing_error();
    }

    if (break_interrupt_occured(line_status)) {
        handle_break_interrupt();
    }
}

void handle_received_data_avail_interrupt() {
    transfer_data_from_receive_buffer_to_host();
}

bool data_is_not_being_transmitted() {
    return false;
}

void move_next_character_to_transmit_hold_reg() {
    // will need to pay attention to
    // character size of the UART
}

void handle_transmitter_holding_reg_empty_interrupt() {
    if (data_is_not_being_transmitted()) {
        return;
    }

    move_next_character_to_transmit_hold_reg();
}

bool clear_to_send(unsigned char modem_status) {
    modem_status += 1;
    return false;
}

void handle_clear_to_send() {}

bool data_set_ready(unsigned char modem_status) {
    modem_status += 1;
    return false;
}

void handle_data_set_ready() {}

bool ring_indicator(unsigned char modem_status) {
    modem_status += 1;
    return false;
}

void handle_ring_indicator() {}

bool data_carrier_detect(unsigned char modem_status) {
    modem_status += 1;
    return false;
}

void handle_data_carrier_detect() {}

void handle_modem_status_interrupt() {
    extern unsigned short MODEM_STATUS_REG;
    unsigned char modem_status = port_byte_in(MODEM_STATUS_REG);
    if (clear_to_send(modem_status)) {
        handle_clear_to_send();
    }

    if (data_set_ready(modem_status)) {
        handle_data_set_ready();
    }

    if (ring_indicator(modem_status)) {
        handle_ring_indicator();
    }

    if (data_carrier_detect(modem_status)) {
        handle_data_carrier_detect();
    }
}

void handle_ns16550a_interrupt() {
    interrupt_type type = get_type_of_interrupt();

    if (type == NONE) {
        // do nothing for now
    } else if (type == RECEIVER_LINE_STATUS) {
        handle_receiver_line_status_interrupt();
    } else if (type == RECEIVED_DATA_AVAIL) {
        handle_received_data_avail_interrupt();
    } else if (type == TRANSMITTER_HOLDING_REG_EMPTY) {
        handle_transmitter_holding_reg_empty_interrupt();
    } else if (type == MODEM_STATUS) {
        handle_modem_status_interrupt();
    } else {
        char * err_msg = "NS16550a interrupt type not recognized.";
        halt_and_display_error_msg(err_msg);
    }
}