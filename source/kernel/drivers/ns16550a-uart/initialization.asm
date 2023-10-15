[bits 32]

;**************** DATA EQUATES *****************

TX_RX_BUFFER_SIZE equ 0x7cf
DOS_ROUTINE_SPEC equ 0x25
DIVISOR_LATCH_ACCESS_CODE equ 0x80
LOWER_DIVISOR equ 0x06
UPPER_DIVISOR equ 0x00
UART_INT_ENABLE_MASK equ 0x0f


;********* ESTABLISH DATA BUFFERS AND RAM REGISTERS ********

string_buf db TX_RX_BUFFER_SIZE dup ("S")
recieve_buf db TX_RX_BUFFER_SIZE dup ("R")
string_buf_end equ string_buf + TX_RX_BUFFER_SIZE
recieve_buf_end equ receive_buf + TX_RX_BUFFER_SIZE

cli

;****** LOAD NEW INTERRUPT SERVICE ROUTINE ***

IRQ_NUM equ 5

extern _install_irq
extern _handle_ns16550a_interrupt

push dword _handle_ns16550a_interrupt
pushw word IRQ_NUM
call _install_irq
add esp, byte 6

;**************** INITIALIZE NS16550A ***********************

%include "source/kernel/drivers/ns16550a-uart/registers.asm"

mov al, DIVISOR_LATCH_ACCESS_CODE    ; set-up access to divisor latch
mov dx, LINE_CONROL_REG
out dx, al

mov al, LOWER_DIVISOR                ; lower divisor latch, 19.2 kbaud
mov dx, DIVISOR_LATCH_LOW
out dx, al

mov al, UPPER_DIVISOR                ; upper divisor latch
mov dx, DIVISOR_LATCH_HIGH
out dx, al

WORD_LEN_7_PARITY_ENABLE_EVEN equ 0x1a
mov al, WORD_LEN_7_PARITY_ENABLE_EVEN
mov dx, LINE_CONROL_REG
out dx, al

Rx_TRIGGER_14_CHARS_DMA_MODE_0 equ 0xc1
mov al, Rx_TRIGGER_14_CHARS_DMA_MODE_0   
mov dx, FIFO_CONTROL_REG
out dx, al

mov al, UART_INT_ENABLE_MASK      ; enable all UART interrupts
mov dx, INTERRUPT_ENABLE_REG
out dx, al

mov dx, LINE_STATUS_REG           ; read the LSR to clear any false
in al, dx                         ; status interrupts

mov dx, MODEM_STATUS_REG          ; read the MSR TO clear any false
in al, dx                         ; modem interrupts


;*************** ENABLE INTERRUPTS **********************

extern _get_primary_pic_int_mask_reg   ; get primary PIC register
call _get_primary_pic_int_mask_reg
mov dx, al                             ; PIC register stored in dx

in al, dx                              ; current IMR is stored in
                                       ; al

INT_ENABLE_MASK equ 0xef               ; enable all existing interrupts
and al, INT_ENABLE_MASK                ; and the UART's interrupt
out dx, al

SET_OUT_2 equ 0x08                     ; set OUT2 to enable buffer between
mov al, SET_OUT_2                      ; the UART and the ICU
                                        
mov dx, MODEM_CONTROL_REG
out dx, al

sti    ; enable CPU interrupts
