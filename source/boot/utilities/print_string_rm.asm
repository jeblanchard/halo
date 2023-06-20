; Prints the string pointed to by BX to BIOS output.
; Follows each string with a space.
print_string:
    pusha
    mov ah, 0x0e                  ; set to BIOS tele-type output,
                                  ; required for INT 10h
    print_char_loop:
        mov al, [bx]              ; print the char in bx
        int 0x10

        inc bx                    ; move to the next char in the string

        cmp byte[bx], 0               ; check for end of string
        jne print_char_loop


    mov al, 0x20              ; print the space
    int 0x10

    popa
    ret
