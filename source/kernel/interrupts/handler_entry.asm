[bits 32]

global _execute_interrupt_handler

; Called by C code to execute
; an interrupt handler.
;
; Input:
;     argument 0 = 32 bit handler address
_execute_interrupt_handler:
    push ebp
    mov ebp, esp
                         
    mov ebx, [ebp + 8]   ; first parameter to function
    
    pusha
    call ebx             ; call handler that was provided
    popa

    leave
    iret
