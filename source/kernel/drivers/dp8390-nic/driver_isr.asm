[bits 32]

;***********************************************************************
; This interrupt service routine responds to transmit, transmit error, and
; receive interrupts (the PTX, TXE, and PRX bits in the INTERRUPT STATUS
; register) produced from the NIC. Upon transmit interrupts, the upper
; layer software is informed of successful or erroneous transmissions;
; upon receive interrupts, packets are removed from the Receive Buffer
; Ring (in local memory) and transferred to the PC.
;***********************************************************************

%include "source/kernel/drivers/dp8390-nic/registers.asm"
%include "source/kernel/drivers/8259a-pic/ports.h"

;byte_count dw    ?
;byte_count dw    0
imr        db    0x1b      ; image of Interrupt Mask register

;***********************************************************************
; Beginning of Interrupt Service Routine
;***********************************************************************

global nic_isr:
    cli

    push ax             ; save registers
    push bx
    push cx
    push dx
    push di
    push si
    push ds
    push es
    push bp

    mov al, 0x0bc ; 0000 1011 1010
    out PRIMARY_PIC_INT_MASK_REG, al         ; turn off IRQ3

    sti

    mov ds, cs                               ; ds = cs

;***********************************************************************
; Read INTERRUPT STATUS REGISTER for receive packets, transmitted
; packets and errored transmitted packets.
;***********************************************************************

%define PACKET_TRANSMITTED_CODE 0x0a

.poll:
    mov dx, INTERRUPT_STATUS
    in al, dx
    test al, 1                             ; packet received?
    jnz packet_received_routine

    test al, PACKET_TRANSMITTED_CODE       ; packet transmitted?
    jz exit_isr                            ; if not, let's exit

    jmp pkt_tx_rt

.exit_isr:
    mov dx, INTERRUPT_MASK_REG                 ; disabling NIC's interrupt
    mov al, 0
    out dx, al

    cli                                    ; disable hardware interrupts

    mov al, 0x0b4                          ; turn IRQ3 back on
    out PRIMARY_PIC_INT_MASK_REG, al

    mov al, 0x63                           ; send 'EOI' for IRQ3
    out PRIMARY_PIC_COMMAND_REG, al

    sti                                    ; re-enable hardware interrupts

    mov dx, INTERRUPT_MASK_REG       ; NOTE: interrupts from the NIC
    mov al, imr                  ; are enabled at this point so
    out dx, al                   ; that the 8259 interrupt
                                 ; controller does not miss any
                                 ; IRQ edges from the NIC
                                 ; (IRQ is edge sensitive)

    pop bp
    pop es
    pop ds
    pop si
    pop di
    pop dx
    pop cx
    pop bx
    pop ax

    iret

%define RING_OVERFLOW_STATUS_CODE 0x10

; Clears out all good packets in local receive buffer ring.
; Bad packets are ignored.
packet_received_routine:
    mov dx, INTERRUPT_STATUS
    in al, dx
    test al, RING_OVERFLOW_STATUS_CODE       ; test for a Ring overflow
    jnz .ring_overflow

    mov al, 1
    out dx, al                               ; reset PRX bit in ISR
    mov ax, next_packet
    mov cx, packet_length
    mov es, seq_recv_pc_buff
    mov di, offset recv_pc_buff

;***********************************************************************
;
; Inform upper layer software of a received packet to be processed
;
;***********************************************************************

; Checks to see if receive buffer ring is empty
check_ring:
    mov dx, BOUNDARY_REG
    in al, dx
    mov ah, al               ; save BOUNDARY_REG in ah
    mov dx, COMMAND_REG
    mov al, 0x62
    out dx, al               ; switched to pg 1 of NIC
    mov dx, CURRENT
    in al, dx
    mov bh, al               ; bh 4 CURRENT PAGE register
    mov dx, COMMAND_REG
    mov al, 0x22
    out dx, al               ; switched back to pg 0
    cmp ah, bh               ; recv buff ring empty?
    jne packet_received_routine
    jmp .poll

;***********************************************************************
;
; The following code is required to recover from a Ring overflow.
; See Sec. 2.0 of datasheet addendum.
;
;***********************************************************************

ring_overflow:
    mov dx, COMMAND_REG
    mov al, 0x21
    out dx, al                   ; put NIC in stop mode

    mov dx, REMOTE_BYTE_COUNT_0
    xor al, al
    out dx, al
    mov dx, REMOTE_BYTE_COUNT_1
    out dx, al

    mov dx, INTERRUPT_STATUS
    mov cx, 0x7fff              ; load time out counter

wait_for_stop:
    in al, dx
    test al, 0x80               ; look for RST bit to be set
    loop wait_for_stop          ; if we fall thru this loop, the RST bit may not get
                                ; set because the NIC was currently transmitting

    mov dx, TRANSMIT_CONFIG_REG
    mov al, 2
    out dx, al                  ; into loopback mode 1
    mov dx, COMMAND_REG
    mov al, 0x22
    out dx, al                  ; into stop mode

    mov ax, next_packet
    mov cx, packet_length
    mov es, seg recv_pc_buff
    mov di, offset recv_pc_buff
    NICtoPC

    mov dx, INTERRUPT_STATUS
    mov al, 0x10
    out dx, al                  ; clear Overflow bit

    mov dx, TRANSMIT_CONFIG_REG
    mov al, tcr
    out dx, al                  ; put TCR back to normal mode
    jmp check_ring


; packet transmit routine (pkt tx rt) -determine status of
; transmitted packet, then checks the transmit-pending
; queue for the next available packet to transmit.

pkt_tx_rt:
    mov dx, INTERRUPT_STATUS
    mov al, 0x0a
    out dx, al                     ; reset PTX and TXE bits in ISR

    mov dx, TRANSMIT_STATUS        ; check for erroneous TX
    in al, dx
    test al, 0x38                  ; is FU, CRS, or ABT bits set in TSR
    jnz bad_tx

;***********************************************************************
; Inform upper layer software of successful transmission
;***********************************************************************

    jmp chk_tx_queue
bad_tx:                          ; in here if bad TX

;***********************************************************************
;
; Inform upper layer software of erroneous transmission
;
;***********************************************************************

chk_tx_queue:
    call Check_Queue            ; see if a packet is in queue
                                ; assume Check Queue will a non-zero
    cmp cx, 0                   ; value in cx and pointer to the
    je poll                     ; packet in DS:SI if packet is
    call DriverSend             ; available. Returns cx = 0 otherwise
    jmp poll
