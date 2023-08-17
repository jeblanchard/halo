[bits 32]

%include "source/kernel/drivers/dp8390-nic/registers.asm"

; This routine will transfer a packet from the RAM
; on the NIC card to the RAM in the host.
;
; Input:
;   ecx = byte count
;   ax = NIC buffer page to transfer from
global nic_to_host:

    push ax                              ; save buffer page

    inc ecx                               ; make even
    and ecx, 0xfffffffe

    mov dx, REMOTE_BYTE_COUNT_0_REG
    mov al, cl
    out dx, al

    mov dx, REMOTE_BYTE_COUNT_1_REG
    mov al, ch
    out dx, al

    pop ax                                  ; get our page back
    
    mov dx, REMOTE_START_ADDRESS_0
    out dx, al                               ; set as low address

    mov dx, REMOTE_START_ADDRESS_1
    mov al, ah
    out dx, al                               ; set as high address

    %define START_MODE_REMOTE_READ 0x0a

    mov dx, COMMAND_REG
    mov al, START_MODE_REMOTE_READ           ; read and start
    out dx, al

    shr ecx, 1                               ; only need to loop half as many times because
                                             ; we will be transferring words at a time
                                             ; to the host

    mov dx, IO_PORT

    extern _start_packet_transfer_from_nic
    call _start_packet_transfer_from_nic

.read_word:
    in ax, dx

    extern _transfer_word_from_nic_to_nic_receive_buffer

    push ax
    call _transfer_word_from_nic_to_nic_receive_buffer
    add esp, byte 2

    loop .read_word

    extern _end_packet_transfer_from_nic
    call _end_packet_transfer_from_nic

    mov dx, INTERRUPT_STATUS_REG
.check_dma:
    in al, dx
    
    %define REMOTE_DMA_COMPLETE 0x40
    test al, REMOTE_DMA_COMPLETE

    jnz .finished
    jmp .check_dma

.finished:
    out dx, al                            ; ensure Remote DMA bit of the ISR
                                          ; is clear (set to 1)

    ret
