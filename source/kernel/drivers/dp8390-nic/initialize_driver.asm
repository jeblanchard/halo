;***********************************************************************
; Initializes the NIC for a typical network system.
;
; Receive Buffer Ring = 0x2600 to 0x4000
; Transmit Buffer = 0x2000 to 0x2600
;***********************************************************************

%include "source/kernel/drivers/dp8390-nic/registers.asm"

%define STOP_MODE_CMD_CODE 0x21
%define INITIAL_RCR_MODE 0
%define INITIAL_TCR_MODE 0
%define INITIAL_DCR_MODE 0x58
%define INITIAL_INTERRUPT_MASK_REG 0x0b

global initialize_driver:

    mov al, STOP_MODE_CMD_CODE         ; start in Stop Mode
    mov dx, COMMAND_REG
    out dx, al

    mov al, INITIAL_DCR_MODE
    mov dx, DATA_CONFIG_REG            ; data configuration register
    out dx, al

    ; Set remote byte count to zero
    xor al, al

    mov dx, REMOTE_BYTE_COUNT_0
    out dx, al                         ; low remote byte count

    mov dx, REMOTE_BYTE_COUNT_1
    out dx, al                         ; high remote byte count

    mov al, INITIAL_RCR_MODE
    mov dx, RECEIVE_CONFIG_REG
    out dx, al

    mov al, 0x20
    mov dx, TRANSMIT_PAGE              ; transmit page start
    out dx, al

    mov al, 0x02
    mov dx, TRANSMIT_CONFIG_REG
    out dx, al                         ; temporarily go into Loopback mode

    mov al, 0x26
    mov dx, PAGE_START                 ; page start
    out dx, al

    mov dx, BOUNDARY_REG                   ; BOUNDARY_REG register
    out dx, al
    mov al, 0x40

    mov dx, PAGE_STOP                  ; page stop
    out dx, al

    mov al, 0x61                       ; go to page 1 registers
    mov dx, COMMAND_REG
    out dx, al

    mov al, 0x26
    mov dx, CURRENT                    ; current page register
    out dx, al

    mov al, 0x22                       ; back to page 0, start mode
    mov dx, COMMAND_REG
    out dx, al

    mov al, 0x0ff
    mov dx, INTERRUPT_STATUS           ; interrupt status register
    out dx, al

    mov al, INITIAL_INTERRUPT_MASK_REG
    mov dx, INTERRUPT_MASK_REG             ; interrupt mask register
    out dx, al

    mov dx, TRANSMIT_CONFIG_REG
    mov al, INITIAL_TCR_MODE
    out dx, al                         ; TCR in normal mode, NIC is now

    ret
