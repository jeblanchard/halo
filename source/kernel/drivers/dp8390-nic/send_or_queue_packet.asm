[bits 32]

%include "source/kernel/drivers/dp8390-nic/registers.asm"

; Transmits the packet passed in or queues up the
; packet if the transmitter is busy.
;
; Input:
;     esi = pointer to packet to be transmitted
;     ecx = byte count of packet
global send_or_queue_packet:

    cli                                  ; disable interrupts

    mov dx, COMMAND_REG
    in al, dx                            ; read NIC COMMAND_REG register

    %define CURRENTLY_TRANSMITTING 0x26

    cmp al, CURRENTLY_TRANSMITTING       ; check if NIC is currently
                                         ; transmitting
    je .queue_packet                     ; if so, queue packet

    push ecx                             ; store byte count

    mov ah, TRANSMIT_BUFFER
    xor al, al                           ; set page that will receive the packet
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

    jmp .finished

.queue_packet:
    extern _queue_packet_for_transmission

    push ecx                                ; push C function parameters
    push esi
    call _queue_packet_for_transmission
    add esp, byte 4

.finished:
    sti                                     ; enable interrupts
    ret
