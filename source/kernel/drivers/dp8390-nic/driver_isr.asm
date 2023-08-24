[bits 32]

%include "source/kernel/drivers/dp8390-nic/registers.asm"
%include "source/kernel/drivers/8259a-pic/ports.h"

%define NIC_IRQ_NUM 3

; This interrupt service routine responds to transmissions, transmission errors, and
; receive interrupts (the PTX, TXE, and PRX bits in the INTERRUPT STATUS
; register) produced from the NIC. Upon transmit interrupts, the upper
; layer software is informed of successful or erroneous transmissions;
; upon receive interrupts, packets are removed from the Receive Buffer
; Ring (in local memory) and transferred to the host.
global nic_isr:
    cli

    push ax
    push bx
    push cx
    push dx
    push di
    push si
    push ds
    push es
    push bp

    in al, PRIMARY_PIC_INT_MASK_REG         ; disable NIC's IRQ
    or al, NIC_IRQ_NUM
    out PRIMARY_PIC_INT_MASK_REG, al

    sti                                     ; re-enable hardware interrupts

%define PACKET_TRANSMITTED 0x0a
%define PACKET_RECEIVED 0x1

; Read Interrupt Status Register for received packets, transmitted
; packets, and erroneous transmitted packets.
.poll:
    mov dx, INTERRUPT_STATUS_REG
    in al, dx
    test al, PACKET_RECEIVED
    jnz .handle_received_packet

    test al, PACKET_TRANSMITTED
    jz .exit

    jmp .handle_transmitted_packet

.exit:

    mov dx, INTERRUPT_MASK_REG              ; save original IMR value
    in al, dx

    og_imr_value: resb 1
    mov og_imr_value, al

    mov al, 0                               ; disable NIC interrupts
    out dx, al

    cli                                     ; disable hardware interrupts


    in al, PRIMARY_PIC_INT_MASK_REG         ; re-enable NIC's IRQ
    xor al, NIC_IRQ_NUM
    out PRIMARY_PIC_INT_MASK_REG, al

    %define ROTATE_ON_NON_SPECIFIC_EOI 0x60
    mov al, ROTATE_ON_NON_SPECIFIC_EOI          ; build EOI command for the NIC's IRQ
    or al, NIC_IRQ_NUM
    out PRIMARY_PIC_COMMAND_REG, al             ; send out EOI command

    sti                                     ; re-enable hardware interrupts

    mov dx, INTERRUPT_MASK_REG              ; NOTE: interrupts from the NIC
    mov al, og_imr_value                    ; are enabled at this point so
    out dx, al                              ; that the 8259 interrupt
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

%define RING_OVERFLOW 0x10
extern _handle_received_packet

; Clears out all good packets in local receive buffer ring.
; Bad packets are ignored.
.handle_received_packet:
    mov dx, INTERRUPT_STATUS_REG
    in al, dx

    test al, RING_OVERFLOW               ; test for ring overflow
    jnz .handle_ring_overflow

    %define PACKET_RECEIVED_BIT 1
    mov al, PACKET_RECEIVED_BIT
    out dx, al

    call nic_to_host

    ; Inform upper layer software that packet has been
    ; been moved to host
    extern _notify_of_received_packet_to_be_processed
    call _notify_of_received_packet_to_be_processed

; Checks to see if receive buffer ring is empty.
.check_ring:
    mov dx, BOUNDARY_REG
    in al, dx
    mov ah, al                                       ; save BOUNDARY_REG in ah

    %define START_MODE_COMPLETE_DMA_PAGE_1 0x62
    mov dx, COMMAND_REG
    mov al, START_MODE_COMPLETE_DMA_PAGE_1
    out dx, al                                       ; switch to page 1 of the NIC

    %define START_MODE_COMPLETE_REMOTE_DMA 0x22;
    mov dx, CURRENT_PAGE_REG
    in al, dx
    mov bh, al                                       ; bh = CURRENT PAGE register
    mov dx, COMMAND_REG
    mov al, START_MODE_COMPLETE_REMOTE_DMA
    out dx, al                                       ; switched back to pg 0

    cmp ah, bh                                       ; recv buff ring empty?
    jne .handle_received_packet
    jmp .poll

.handle_ring_overflow:
    mov dx, COMMAND_REG
    mov al, 0x21
    out dx, al                                ; put NIC in stop mode

    mov dx, REMOTE_BYTE_COUNT_0_REG
    xor al, al
    out dx, al
    mov dx, REMOTE_BYTE_COUNT_1_REG
    out dx, al

    mov dx, INTERRUPT_STATUS_REG
    mov cx, 0x7fff                            ; load time out counter

.wait_for_reset:
    in al, dx

    %define RESET_STATUS 0x80
    test al, RESET_STATUS           ; test if RST bit has been set
    loop .wait_for_reset            ; if we fall through this loop,
                                    ; the RST bit may not get set
                                    ; because the NIC was currently
                                    ; transmitting

    mov dx, TRANSMIT_CONFIG_REG     ; save original value of the
                                    ; Transmission Config. Register
    og_tcr_value: resb 1
    in al, dx
    mov og_tcr_value, al

    %define INTERNAL_LOOPBACK_MODE_1 0x2
    mov al, INTERNAL_LOOPBACK_MODE_1
    out dx, al                                          ; go into loopback mode 1

    mov dx, COMMAND_REG
    mov al, START_MODE_COMPLETE_REMOTE_DMA
    out dx, al                                          ; back into start mode

    call nic_to_host

    %define OVERFLOW_BIT 0x10
    mov dx, INTERRUPT_STATUS_REG
    mov al, OVERFLOW_BIT
    out dx, al

    mov dx, TRANSMIT_CONFIG_REG
    mov al, tcr
    out dx, al                                    ; put TCR back to normal mode

    jmp .check_ring


; Determines status of transmitted packet then
; checks the transmit-pending queue for the
; next available packet to transmit.
.handle_transmitted_packet:
    mov dx, INTERRUPT_STATUS_REG
    mov al, 0x0a
    out dx, al                     ; reset PTX and TXE bits in ISR

    mov dx, TRANSMIT_STATUS        ; check for erroneous TX
    in al, dx
    test al, 0x38                  ; is FU, CRS, or ABT bits set in TSR
    jnz .handle_bad_transmission

    extern _notify_of_successful_transmission
    call _notify_of_successful_transmission

    jmp check_transmission_queue

.handle_bad_transmission:
    extern _notify_of_erroneous_transmission
    call _notify_of_erroneous_transmission

.check_transmission_queue:
    call check_queue
    cmp cx, 0
    je .poll

    call transmit_next_packet_in_queue
    jmp .poll

%include "source/kernel/drivers/dp8390-nic/transmit_next_packet_in_queue.asm"
%include "source/kernel/drivers/dp8390-nic/check_queue.asm"
%include "source/kernel/drivers/dp8390-nic/nic_to_host.asm"
