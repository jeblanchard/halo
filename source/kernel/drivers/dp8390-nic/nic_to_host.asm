[bits 32]

%include "source/kernel/drivers/dp8390-nic/registers.asm"

; This routine will transfer a packet from the RAM
; on the NIC card to the RAM in the host.
global nic_to_host
nic_to_host:

    mov dx, REMOTE_BYTE_COUNT_1_REG           ; this needs to be set in
                                              ; order for the NIC to
                                              ; correctly execute the
                                              ; Send Packet Command

    mov al, 0x0f
    out dx, al

    %define START_MODE_SEND_PACKET 0x1a

    mov dx, COMMAND_REG
    mov al, START_MODE_SEND_PACKET
    out dx, al

    extern _start_packet_transfer_from_nic
    call _start_packet_transfer_from_nic

    extern _transfer_word_from_nic_to_nic_receive_buffer

.move_word_to_host:
    mov dx, IO_PORT
    in ax, dx

    push ax
    call _transfer_word_from_nic_to_nic_receive_buffer
    add esp, byte 2

    %define REMOTE_DMA_COMPLETE 0x40
.check_dma:
    mov dx, INTERRUPT_STATUS_REG
    in al, dx

    test al, REMOTE_DMA_COMPLETE
    jnz .finished
    jmp .move_word_to_host

.finished:
    out dx, al                                ; clear the REMOTE DMA COMPLETE
                                              ; bit

    extern _end_packet_transfer_from_nic
    call _end_packet_transfer_from_nic

    ret
