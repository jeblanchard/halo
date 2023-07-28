; A boot sector that boots a C kernel in 32-bit protected mode

; This is where the bootsector is expected to be loaded by the BIOS.
[org 0x7c00]

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

KERNEL_OFFSET equ 0x1000            ; This is the memory offset to which we will load our kernel

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

; Switch to protected mode
switch_to_pm:

    ; We must switch off hardware interrupts until we have
    ; set-up the PM IDT, otherwise the PIT will
    ; trigger a hardware interrupt.
    cli

    ; Load our global descriptor table, which defines
    ; the protected mode segments (e.g. for code and data)
    lgdt [gdt_descriptor]

    ; To make the switch to protected mode, we set
    ; the first bit of CR0, a control register
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ; Make a far jump (i.e. to a new segment) to our 32-bit
    ; code. This also forces the CPU to flush its cache of
    ; pre-fetched and real-mode decoded instructions, which can
    ; cause problems.
    jmp CODE_SEG:init_pm

[bits 32]

; Initialise registers and the stack once in PM.
init_pm:

    ; Now in PM, our old segments are meaningless,
    ; so we point our segment registers to the
    ; data selector we defined in our GDT
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Update our stack position so it is right
    ; at the top of the free space.
    mov ebp, 0x90000
    mov esp, ebp

    ; Finally, call some well-known label
    call BEGIN_PM

; This is where we arrive after switching to and initialising protected mode.

BEGIN_PM:

mov ebx, MSG_PROT_MODE              ; Use our 32-bit print routine to
call print_string_pm                ; announce we are in protected mode

call KERNEL_OFFSET                  ; Now jump to the address of our loaded
                                    ; kernel code

jmp $                               ; We should never reach here, but we should
                                    ; hang if so.

; Global variables
BOOT_DRIVE                   db 0
MSG_REAL_MODE                db "Started in 16-bit Real Mode.", 0
MSG_PROT_MODE                db "Successfully landed in 32-bit Protected Mode.", 0
DEBUG_SET_BOOT_DRIVE         db "Set up boot drive.", 0
IN_LOAD_KERNEL               db "In load kernel.", 0

; Boot sector padding
times 510-($-$$) db 0
dw 0xaa55
