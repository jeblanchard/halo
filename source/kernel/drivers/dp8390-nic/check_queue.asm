[bits 32]

; Checks if a packet is in queue.
;
; Returns:
;     cx = byte count of packet in queue
;        = 0 when queue is empty
;     ds:esi = pointer to the packet
check_queue:
    ret
