[bits 32]

; Checks if a packet is in queue to
; be transmitted.
;
; Returns:
;     cx = byte count of packet in queue
;        = 0 when queue is empty
;     ds:esi = pointer to the packet
check_queue:
    ret

;
extern _check_queue