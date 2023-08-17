[bits 32]

%include "source/kernel/drivers/dp8390-nic/registers.asm"

; Transfers a packet from the host's RAM
; to the local RAM on the NIC card.
;
; Input:
;   esi = address of packet to be transferred
;   ecx = byte count
;   ax = NIC buffer page to transfer to
load_packet_from_host:

    push ax                               ; save buffer page

    inc ecx                               ; make byte count even
    and ecx, 0xfffffffe

    mov dx, REMOTE_BYTE_COUNT_0_REG           ; set byte count low byte
    mov al, cl
    out dx, al

    mov dx, REMOTE_BYTE_COUNT_1_REG           ; set byte count high byte
    mov al, ch
    out dx, al

    pop ax                                    ; get our page back

    mov dx, REMOTE_START_ADDRESS_0
    out dx, al                                ; set as lo address

    mov dx, REMOTE_START_ADDRESS_1
    mov al, ah                                ; set as hi address
    out dx, al

    %define START_MODE_REMOTE_WRITE 0x12
    mov dx, COMMAND_REG
    mov al, START_MODE_REMOTE_WRITE           ; write and start
    out dx, al

    mov dx, IO_PORT
    shr ecx, 1                                ; because we will be transferring
                                              ; words at a time, we only need to
                                              ; loop half of the byte count

    extern _process_next_transmission_word
    extern _start_packet_transfer_to_nic

    call _start_packet_transfer_to_nic

.load_word_from_host:

    call _process_next_transmission_word

    out dx, ax                         ; write to IO_PORT on NIC board

    loop .load_word_from_host          ; decrement ecx (byte count) and
                                       ; check if ecx = 0 (which would
                                       ; mean we've loaded all words from
                                       ; the host)

    extern _end_packet_transfer_to_nic
    call _end_packet_transfer_to_nic

    mov ecx, 0

    mov dx, INTERRUPT_STATUS_REG
    %define REMOTE_DMA_COMPLETE 0x40

.check_dma:
    in al, dx
    test al, REMOTE_DMA_COMPLETE       ; Check if DMA has completed
    jnz .finished                      ; if so, we're done
    jmp .check_dma                     ; loop until the DMA has finished

.finished:
    mov dx, INTERRUPT_STATUS_REG
    mov al, REMOTE_DMA_COMPLETE        ; clear DMA interrupt bit in ISR
    out dx, al

    clc
    ret
