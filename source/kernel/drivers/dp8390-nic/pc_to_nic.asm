%include "source/kernel/drivers/dp8390-nic/registers.asm"

; Transfers a packet from the host's RAM
; to the local RAM on the NIC card.
;
; Input:
;   ds:si = packet to be transferred
;   cx = byte count
;   ax = NIC buffer page to transfer to
load_packet_from_host:

    push ax                               ; save buffer page

    inc cx                                ; make byte count even
    and cx, 0x0fffe

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
    shr cx, 1                                 ; need to loop half as many times

.load_word_from_host:                  ; because of word-wide transfers
    lodsw                              ; load word from ds:si
    out dx, ax                         ; write to IO_PORT on NIC board
    loop .load_word_from_host

    mov cx, 0

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

