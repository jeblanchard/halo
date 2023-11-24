; A small wrapper that loads
; the CR3 register. Uses
; a 32 bit value on the
; stack.
global _load_pdbr_asm:
    push ebp
    mov ebp, esp
    mov cr3, [ebp + 8]
    
    leave
    ret