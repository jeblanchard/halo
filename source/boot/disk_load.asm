; Load disk sectors to ES:BX from
; the specified drive.
;
; Input:
;   AL = number of sectors to read
;   DL = the boot drive number
disk_load:
    pusha

    mov ah, 0x02                    ; Function selection: DISK - READ SECTOR(S)
                                    ; INTO MEMORY
    mov ch, 0x00                    ; Low eight bits of cylinder number
    mov cl, 0x02                    ; Start reading from second sector (i.e.
                                    ; after the boot sector)
    mov dh, 0x00                    ; Select head 0

    int 0x13                        ; BIOS interrupt
    jc disk_error                   ; Jump if error (i.e. carry flag was set)

    popa
    ret

disk_error:
    pusha

    mov bx, DISK_ERROR_MSG
    call print_string

    ; Print the disk read status and number of sectors
    ; transferred by the BIOS.
    mov dx, ax
    call print_hex

    popa
    jmp $

%include "source/boot/utils/print_hex_rm.asm"

; Variables
DISK_ERROR_MSG db "Disk read error!", 0
