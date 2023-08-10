; Initializes the NIC for a typical network system.

%include "source/kernel/drivers/dp8390-nic/registers.asm"

%define STOP_MODE_ABORT_PAGE_0_CMD_CODE  0x21 ; 0010 0001
%define STOP_MODE_ABORT_PAGE_1_CMD_CODE  0x61 ; 0110 0001
%define START_MODE_ABORT_PAGE_0_CMD_CODE 0x22 ; 0010 0010

%define INTERNAL_LOOPBACK_MODE_CODE 0x02 ; 0000 0010

%define CLEARED_RCR 0
%define TCR_NORMAL_MODE 0

%define NORMAL_AUTO_INIT_4_BYTES 0x58 ; 0101 1000

%define INITIAL_INTERRUPT_MASK 0x0b ; 0000 1011

%define ISR_CLEAR 0xff

%define RECEIVE_BUFFER_RING_START_UPPER_8_BITS 0x26
%define RECEIVE_BUFFER_RING_STOP_UPPER_8_BITS 0x40

%define TRANSMIT_BUFFER_START_UPPER_8_BITS 0x20

%define INITIAL_REMOTE_BYTE_COUNT_LOWER_8_BITS 0
%define INITIAL_REMOTE_BYTE_COUNT_UPPER_8_BITS 0

global initialize_driver:

    mov al, STOP_MODE_ABORT_PAGE_0_CMD_CODE
    mov dx, COMMAND_REG
    out dx, al

    mov al, NORMAL_AUTO_INIT_4_BYTES
    mov dx, DATA_CONFIG_REG
    out dx, al

    mov dx, REMOTE_BYTE_COUNT_0_REG
;    out dx, INITIAL_REMOTE_BYTE_COUNT_LOWER_8_BITS

    mov dx, REMOTE_BYTE_COUNT_1_REG
;    out dx, INITIAL_REMOTE_BYTE_COUNT_UPPER_8_BITS

    mov al, CLEARED_RCR
    mov dx, RECEIVE_CONFIG_REG
    out dx, al

    mov al, TRANSMIT_BUFFER_START_UPPER_8_BITS
    mov dx, TRANSMIT_PAGE_START_REG
    out dx, al

    mov al, INTERNAL_LOOPBACK_MODE_CODE
    mov dx, TRANSMIT_CONFIG_REG
    out dx, al

    mov al, RECEIVE_BUFFER_RING_START_UPPER_8_BITS
    mov dx, PAGE_START_REG
    out dx, al
    mov dx, BOUNDARY_REG
    out dx, al

    mov al, RECEIVE_BUFFER_RING_STOP_UPPER_8_BITS
    mov dx, PAGE_STOP
    out dx, al

    mov al, STOP_MODE_ABORT_PAGE_1_CMD_CODE
    mov dx, COMMAND_REG
    out dx, al

    mov al, RECEIVE_BUFFER_RING_START_UPPER_8_BITS
    mov dx, CURRENT_PAGE_REG
    out dx, al

    mov al, START_MODE_ABORT_PAGE_0_CMD_CODE
    mov dx, COMMAND_REG
    out dx, al

    mov al, ISR_CLEAR
    mov dx, INTERRUPT_STATUS_REG
    out dx, al

    mov al, INITIAL_INTERRUPT_MASK
    mov dx, INTERRUPT_MASK_REG
    out dx, al

    mov dx, TRANSMIT_CONFIG_REG
    mov al, TCR_NORMAL_MODE
    out dx, al

    ret
