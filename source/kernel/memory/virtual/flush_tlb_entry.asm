; A small function that flushes
; the TLB. Uses a 32 bit
; value on the stack.
global _flush_tlb_entry:
    push ebp
    mov ebp, esp

    mov eax, [ebp + 8]
    invlpg eax
    
    leave
    ret
