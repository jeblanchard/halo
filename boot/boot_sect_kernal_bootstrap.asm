; A boot sector that boots a C kernel in 32-bit protected mode
[org 0x7c00]
KERNEL_OFFSET equ 0x1000            ; This is the memory offset to which we will load our kernel

mov [BOOT_DRIVE], dl                ; BIOS stores our boot drive in DL, so it’s
                                    ; best to remember this for later.
mov bx, DEBUG_SET_BOOT_DRIVE
call print_string

mov bp, 0x9000                      ; Set-up the stack.
mov sp, bp

mov bx, MSG_REAL_MODE               ; Announce that we are starting
call print_string                   ; booting from 16-bit real mode

call load_kernel                ; Load our kernel

call switch_to_pm               ; Switch to protected mode, from which
                                ; we will not return
jmp $

%include "boot/utilities/print_string.asm"
%include "boot/utilities/disk_load.asm"
%include "boot/pm/gdt.asm"
%include "boot/pm/print_string_pm.asm"
%include "boot/pm/switch_to_pm.asm"

[bits 16]

load_kernel:

    ; Set-up parameters for our disk_load routine, so
    ; that we load the first 15 sectors (excluding
    ; the boot sector) from the boot disk (i.e. our
    ; kernel code) to address KERNEL_OFFSET
    mov bx, KERNEL_OFFSET
    mov dh, 15
    mov dl, [BOOT_DRIVE]

    call disk_load

    ret

[bits 32]

; This is where we arrive after switching to and initialising protected mode.

BEGIN_PM:

mov ebx, MSG_PROT_MODE              ; Use our 32-bit print routine to
call print_string_pm                ; announce we are in protected mode

call KERNEL_OFFSET                  ; Now jump to the address of our loaded
                                    ; kernel code, assume the brace position,
                                    ; and cross your fingers. Here we go!

jmp $                               ; Hang.

; Global variables
BOOT_DRIVE                   db 0
MSG_REAL_MODE                db "Started in 16-bit Real Mode", 0
MSG_PROT_MODE                db "Successfully landed in 32-bit Protected Mode", 0
DEBUG_SET_BOOT_DRIVE         db "Set up boot drive.", 0

; Boot sector padding
times 510-($-$$) db 0
dw 0xaa55
