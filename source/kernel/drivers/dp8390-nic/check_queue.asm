[bits 32]

; Checks if a packet is in queue to
; be transmitted.
;
; Returns:
;     ecx = byte count of packet in queue
;         = 0 when queue is empty
check_queue:
    call _get_byte_count_of_next_packet_in_transmission_queue
    mov ecx, eax

    ret

extern _get_byte_count_of_next_packet_in_transmission_queue
