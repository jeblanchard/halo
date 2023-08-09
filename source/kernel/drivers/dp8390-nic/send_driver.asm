%include "source/kernel/drivers/dp8390-nic/registers.asm"

%define CURRENTLY_TRANSMITTING 0x26

; Transmits the packet passed in or queues up the
; packet if the transmitter is busy.
;
; Input:
;     ds:si = packet to be transmitted
;     cx = byte count of packet
global send_or_queue_packet:

    cli                                  ; disable interrupts

    mov dx, COMMAND_REG
    in al, dx                            ; read NIC COMMAND_REG register

    cmp al, CURRENTLY_TRANSMITTING       ; check if NIC is currently
                                         ; transmitting
    je .queue_packet                     ; if so, queue packet

    push cx                              ; store byte count
    mov ah, TRANSMIT_BUFFER
    xor al, al                           ; set page that will receive the packet
    call pc_to_nic                       ; transfer packet to NIC buffer RAM

    mov dx, TRANSMIT_PAGE_START_REG
    mov al, TRANSMIT_BUFFER
    out dx, al                           ; set NIC transmit page

    pop cx                               ; get byte count back

    mov dx, TRANSMIT_BYTE_COUNT_0
    mov al, cl
    out dx, al                           ; set transmit byte count 0 on NIC

    mov dx, TRANSMIT_BYTE_COUNT_1
    mov al, ch
    out dx, al                           ; set transmit byte count 1 on NIC

    mov dx, COMMAND_REG
    mov al, 0x26
    out dx, al                           ; issue transmit to COMMAND_REG register

    jmp .finished

.queue_packet:
    call queue_packet

.finished:                               ; enable interrupts
    sti
    ret
