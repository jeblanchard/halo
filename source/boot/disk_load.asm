; load DH sectors to ES:BX from drive DL
disk_load:
    push dx                         ; Store DX on stack so later we can recall
                                    ; how many sectors were request to be read,
                                    ; even if it is altered in the meantime

    mov ah, 0x02                    ; BIOS read sector function
    mov al, dh                      ; Read DH sectors
    mov ch, 0x00                    ; Select cylinder 0
    mov dh, 0x00                    ; Select head 0
    mov cl, 0x02                    ; Start reading from second sector (i.e.
                                    ; after the boot sector)

    int 0x13                        ; BIOS interrupt

    jc disk_error                   ; Jump if error (i.e. carry flag set)
    pop dx                          ; Restore DX from the stack

    cmp dh, al                      ; if AL (sectors read) !=
                                    ; DH (sectors expected)
    jne disk_error

    ret

disk_error:
    mov bx, DISK_ERROR_MSG
    call print_string

    pusha

    ; clear low bits of ax so we can easily pass
    ; ax to our 16-bit print_hex function
    ; ah will contain the disk read status
    ; code
    mov al, 0x00

    mov dx, ax
    call print_hex

    popa

    jmp $

%include "source/boot/utilities/print_hex_rm.asm"

; Variables
DISK_ERROR_MSG db "Disk read error!", 0
