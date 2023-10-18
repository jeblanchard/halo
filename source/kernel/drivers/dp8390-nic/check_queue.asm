[bits 32]

; Checks if a packet is in queue to
; be transmitted.
;
; Returns:
;     ecx = byte count of packet in queue
;         = 0 when queue is empty
global check_queue
check_queue:
    call _view_byte_count_of_next_packet_to_transmit
    mov ecx, eax

    ret

extern _view_byte_count_of_next_packet_to_transmit
