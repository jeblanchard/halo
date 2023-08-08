;***********************************************************************
; Initializes the NIC for a typical network system.
; Receive Buffer Ring 4 2600h to 4000h
; Transmit Buffer 4 2000h to 2600h
;***********************************************************************

%include "source/kernel/drivers/dp8390-nic/registers.asm"

;CGroup group Code
;Code segment para public 'Code'
;     assume cs:CGroup, ds:CGroup, es:nothing, ss:nothing

rcr db 0              ; value for Recv config. reg
tcr db 0              ; value for trans. config. reg
dcr db 0x58           ; value for data config. reg
imr db 0x0b           ; value for interrupt mask reg

;DriverInitialize proc near
global initialize_driver:

    mov al, 0x21                      ; stop mode
    mov dx, COMMAND
    out dx, al

    mov al, dcr
    mov dx, DATA_CONFIGURATION        ; data configuration register
    out dx, al

    mov dx, REMOTE_BYTE_COUNT_0
    xor al, al
    out dx, al                        ; low remote byte count
    mov dx, REMOTE_BYTE_COUNT_1
    out dx, al                        ; high remote byte count

    mov al, rcr
    mov dx, RECEIVE_CONFIGURATION     ; receive configuration register
    out dx, al

    mov al, 0x20
    mov dx, TRANSMIT_PAGE             ; transmit page start
    out dx, al

    mov al, 0x02
    mov dx, TRANSMIT_CONFIGURATION
    out dx, al                        ; temporarily go into Loopback mode

    mov al, 0x26
    mov dx, PAGE_START                ; page start
    out dx, al

    mov dx, BOUNDARY                  ; boundary register
    out dx, al
    mov al, 0x40

    mov dx, PAGE_STOP                 ; page stop
    out dx, al

    mov al, 0x61                      ; go to page 1 registers
    mov dx, COMMAND
    out dx, al

    mov al, 0x26
    mov dx, CURRENT                   ; current page register
    out dx, al

    mov al, 0x22                      ; back to page 0, start mode
    mov dx, COMMAND
    out dx, al

    mov al, 0x0ff
    mov dx, INTERRUPT_STATUS          ; interrupt status register
    out dx, al

    mov al, imr
    mov dx, INTERRUPT_MASK            ; interrupt mask register
    out dx, al

    mov dx, TRANSMIT_CONFIGURATION
    mov al, tcr
    out dx, al                        ; TCR in norma1 mode, NIC is now

    ; ready for reception
    ret

;DriverInitialize endp
;Code ends
;end
