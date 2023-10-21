[bits 32]

;**************** DATA EQUATES *****************

DOS_ROUTINE_SPEC equ 0x25
DIVISOR_LATCH_ACCESS_CODE equ 0x80
LOWER_DIVISOR equ 0x06
UPPER_DIVISOR equ 0x00
UART_INT_ENABLE_MASK equ 0x0f

; Pause interrupts during initialization
cli

;****** LOAD NEW INTERRUPT SERVICE ROUTINE ***

IRQ_NUM equ 5

extern _install_irq
extern _handle_ns16550a_interrupt

push dword _handle_ns16550a_interrupt
push word IRQ_NUM
call _install_irq
add esp, byte 6

;**************** INITIALIZE NS16550A ***********************

extern _LINE_CONROL_REG

mov al, DIVISOR_LATCH_ACCESS_CODE    ; set-up access to divisor latch
mov dx, _LINE_CONROL_REG
out dx, al

extern _DIVISOR_LATCH_LOW

mov al, LOWER_DIVISOR                ; lower divisor latch, 19.2 kbaud
mov dx, _DIVISOR_LATCH_LOW
out dx, al

extern _DIVISOR_LATCH_HIGH

mov al, UPPER_DIVISOR                ; upper divisor latch
mov dx, _DIVISOR_LATCH_HIGH
out dx, al

extern _LINE_CONROL_REG

WORD_LEN_7_PARITY_ENABLE_EVEN equ 0x1a
mov al, WORD_LEN_7_PARITY_ENABLE_EVEN
mov dx, _LINE_CONROL_REG
out dx, al

extern _FIFO_CONTROL_REG

Rx_TRIGGER_14_CHARS_DMA_MODE_0 equ 0xc1
mov al, Rx_TRIGGER_14_CHARS_DMA_MODE_0   
mov dx, _FIFO_CONTROL_REG
out dx, al

extern _INTERRUPT_ENABLE_REG

mov al, UART_INT_ENABLE_MASK      ; enable all UART interrupts
mov dx, _INTERRUPT_ENABLE_REG
out dx, al

extern _LINE_STATUS_REG

mov dx, _LINE_STATUS_REG          ; read the LSR to clear any false
in al, dx                         ; status interrupts

extern _MODEM_STATUS_REG

mov dx, _MODEM_STATUS_REG          ; read the MSR TO clear any false
in al, dx                          ; modem interrupts


;*************** ENABLE INTERRUPTS **********************

extern _get_primary_pic_int_mask_reg   ; get primary PIC register
call _get_primary_pic_int_mask_reg

mov dx, ax                             ; PIC INT mask register stored in ax

in al, dx                              ; get current INT mask

INT_ENABLE_MASK equ 0xef               ; enable all existing interrupts
and al, INT_ENABLE_MASK                ; and the UART's interrupt
out dx, al

SET_OUT_2 equ 0x08                     ; set OUT2 to enable buffer between
mov al, SET_OUT_2                      ; the UART and the ICU

extern _MODEM_CONTROL_REG                      
mov dx, _MODEM_CONTROL_REG
out dx, al

sti    ; resume CPU interrupts
