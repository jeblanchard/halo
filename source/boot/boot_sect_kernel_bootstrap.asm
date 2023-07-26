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

mov bx, IN_LOAD_KERNEL              ; Load our kernel
call print_string

; Set-up parameters for our disk_load routine, so
; that we load the first 15 sectors (excluding
; the boot sector) from the boot disk (i.e. our
; kernel code) to address KERNEL_OFFSET
mov bx, KERNEL_OFFSET
mov dh, 22
mov dl, [BOOT_DRIVE]

call disk_load

call switch_to_pm               ; Switch to protected mode, from which
                                ; we will not return
jmp $

%include "source/boot/utilities/print_string_rm.asm"
%include "source/boot/disk_load.asm"
%include "source/boot/basic_gdt.asm"
%include "source/boot/utilities/print_string_pm.asm"
%include "source/boot/switch_to_pm.asm"

[bits 32]

; This is where we arrive after switching to and initialising protected mode.

BEGIN_PM:

mov ebx, MSG_PROT_MODE              ; Use our 32-bit print routine to
call print_string_pm                ; announce we are in protected mode

mov	eax, 0x2BADB002		            ; Multiboot specs say eax should be this
mov	ebx, 0
mov	edx, [ImageSize]

push dword boot_info
call ebp               	            ; Execute Kernel
add	esp, 4

call KERNEL_OFFSET                  ; Now jump to the address of our loaded
                                    ; kernel code, assume the brace position,
                                    ; and cross your fingers. Here we go!

jmp $                               ; Hang.

; Global variables
BOOT_DRIVE                   db 0
MSG_REAL_MODE                db "Started in 16-bit Real Mode.", 0
MSG_PROT_MODE                db "Successfully landed in 32-bit Protected Mode.", 0
DEBUG_SET_BOOT_DRIVE         db "Set up boot drive.", 0
IN_LOAD_KERNEL               db "In load kernel", 0

; Boot sector padding
times 510-($-$$) db 0
dw 0xaa55
