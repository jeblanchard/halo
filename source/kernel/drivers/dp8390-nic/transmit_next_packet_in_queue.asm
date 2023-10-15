[bits 32]

%include "source/kernel/drivers/dp8390-nic/registers.asm"

; Transmits the next packet in the queue if the
; NIC is not busy.
;
; Input:
;     ecx = byte count of packet
global transmit_next_packet_in_queue:

    cli                                  ; disable interrupts

    mov dx, COMMAND_REG
    in al, dx                            ; read NIC COMMAND_REG register

    %define CURRENTLY_TRANSMITTING 0x26

    cmp al, CURRENTLY_TRANSMITTING       ; check if NIC is currently
                                         ; transmitting
    je .finished                         ; if so, we do nothing (we leave the
                                         ; packet in the queue)

    push ecx                             ; store byte count

    mov ah, TRANSMIT_BUFFER
    xor al, al                           ; set page that will receive the packet

    extern load_packet_from_host
    call load_packet_from_host           ; transfer packet to NIC buffer RAM

    mov dx, TRANSMIT_PAGE_START_REG
    mov al, TRANSMIT_BUFFER
    out dx, al                           ; set NIC transmit page

    pop ecx                              ; get byte count back

    mov dx, TRANSMIT_BYTE_COUNT_0
    mov al, cl
    out dx, al                           ; set transmit byte count 0 on NIC

    mov dx, TRANSMIT_BYTE_COUNT_1
    mov al, ch
    out dx, al                           ; set transmit byte count 1 on NIC

    mov dx, COMMAND_REG
    mov al, CURRENTLY_TRANSMITTING
    out dx, al                           ; issue transmit to COMMAND_REG register

.finished:
    sti                                         ; enable interrupts
    ret
