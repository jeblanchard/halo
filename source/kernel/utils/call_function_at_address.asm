global _call_function_at_address

; Calls the C function at
; the provided address.
_call_function_at_address:
    push ebp
    mov ebp, esp

    mov ebx, [ebp + 8]    ; first parameter

    call ebx              ; call the function provided

    leave
    ret
