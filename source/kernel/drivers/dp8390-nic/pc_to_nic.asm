;***********************************************************************
; This routine will transfer a packet from the PC's RAM
; to the local RAM on the NIC card.
;
; Input:
; ds:si = packet to be transferred
; cx = byte count
; ax = NIC buffer page to transfer to
;***********************************************************************
;PCtoNIC proc far

%include "source/kernel/drivers/dp8390-nic/registers.asm"

global pc_to_nic:

    push ax                               ; save buffer address
    inc cx                                ; make even
    and cx, 0x0fffe
    mov dx, REMOTE_BYTE_COUNT_0           ; set byte count low byte
    mov al, cl
    out dx, al
    mov dx, REMOTE_BYTE_COUNT_1           ; set byte count high byte
    mov al, ch
    out dx, al
    pop ax                                ; get our page back
    mov dx, REMOTE_START_ADDRESS_0
    out dx, al                            ; set as lo address
    mov dx, REMOTE_START_ADDRESS_1
    mov al, ah                            ; set as hi address
    out dx, al
    mov dx, COMMAND_REG
    mov al, 0x12                          ; write and start
    out dx, al
    mov dx, IO_PORT
    shr cx, 1                             ; need to loop half as many times

.writing_word:                         ; because of word-wide transfers
    lodsw                              ; load word from ds:si
    out dx, ax                            ; write to IO_PORT on NIC board
    loop Writing_Word
    mov cx, 0
    mov dx, INTERRUPT_STATUS

.check_dma:
    in al, dx
    test al, 0x40                     ; dma done ???
    jnz .to_nic_end                      ; if so, go to NICEND
    jmp .check_dma                      ; loop until done

.to_nic_end:
    mov dx, INTERRUPT_STATUS
    mov al, 0x40                      ; clear DMA interrupt bit in ISR
    out dx, al
    clc
    ret

;PCtoNIC endp
