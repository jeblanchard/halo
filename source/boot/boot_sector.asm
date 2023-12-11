%define BIOS_BOOT_SECTOR_MEM_LOCATION 0x7c00
[org BIOS_BOOT_SECTOR_MEM_LOCATION]

mov bp, 0x9000                      ; First of all, we set-up the stack.
mov sp, bp

mov bx, MSG_REAL_MODE               ; Announce that booting has
call print_string                   ; started.

; Load our kernel and Multiboot2 code from the disk drive into memory.
load_kernel_from_drive:
    mov [BOOT_DRIVE], dl                ; BIOS stores our boot drive in DL

    %include "source/boot/kernel_phys_addr.asm"
    MULTIBOOT2_OFFSET equ _KERNEL_PHYS_ADDRESS        ; This is the memory offset to which we
                                                      ; will load our kernel.
 
    mov bx, MULTIBOOT2_OFFSET           ; The destination base address in
                                        ; memory.
    mov al, 50                          ; Number of sectors we will load.
    mov dl, [BOOT_DRIVE]                ; Populate DL with the boot drive number.
    call disk_load

    mov bx, IN_LOAD_KERNEL              ; Announce we've loaded our kernel into
    call print_string                   ; memory.

jmp MULTIBOOT2_OFFSET          ; Jump to the address of our
                               ; Multiboot2 code.

jmp $                           ; We should never arrive here,
                                ; but we'll hang if so.

%include "source/boot/utils/print_string_rm.asm"
%include "source/boot/disk_load.asm"

; Global variables
BOOT_DRIVE                   db 0
MSG_REAL_MODE                db "Started in 16-bit Real Mode.", 0
IN_LOAD_KERNEL               db "Loaded kernel and multiboot from drive.", 0

; Boot sector padding
times 510-($-$$) db 0
dw 0xaa55
