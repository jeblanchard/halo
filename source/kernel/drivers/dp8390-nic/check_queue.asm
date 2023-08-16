[bits 32]

; Checks if a packet is in queue to
; be transmitted.
;
; Returns:
;     ecx = byte count of packet in queue
;         = 0 when queue is empty
;     esi = pointer to the packet
check_queue:
    call _get_byte_count_of_next_packet_in_transmission_queue
    mov ecx, eax

    call _get_address_of_next_packet_data
    mov esi, eax

    ret

extern _get_byte_count_of_next_packet_in_transmission_queue
extern _get_address_of_next_packet_data
