; Collects system info and then enters our kernel.

[bits 16]

multiboot2:

    struc multiboot2_info
        .total_size                         resd    1
        .reserved                           resd    1

        .mem_info_type                      resd    1
        .mem_info_size                      resd    1
        .num_kb_in_mem	                    resd	1

        .mem_map_info_type                  resd    1
        .mem_map_info_size                  resd    1
        .entry_size                         resd    1
        .entry_version                      resd    1
        .mem_map_entry_list_base_addr       resd    1
        .mem_map_num_entries                resd    1

        .terminating_tag_type               resd    1
        .terminating_tag_size               resd    1
    endstruc

    multiboot2_info_instance: resb multiboot2_info_size

    memory_map_entries: resb 10 * memory_map_entry_size

    after_memory_map:

    fill_multiboot2_fixed_part:
        mov dword [multiboot2_info_instance + multiboot2_info.total_size], multiboot2_info_size
        mov dword [multiboot2_info_instance + multiboot2_info.reserved], 0

    fill_multiboot2_mem_info:
        mov dword [multiboot2_info_instance + multiboot2_info.mem_info_type], 4
        mov dword [multiboot2_info_instance + multiboot2_info.mem_info_size], 16

        mov bx, BEFORE_NUM_KB
        call print_string

        call get_num_kb_in_system

        mov bx, AFTER_NUM_KB
        call print_string

        mov dword [multiboot2_info_instance + multiboot2_info.num_kb_in_mem], ecx

    fill_multiboot2_mem_map_info:
        mov dword [multiboot2_info_instance + multiboot2_info.mem_map_info_type], 6
        mov dword [multiboot2_info_instance + multiboot2_info.mem_map_info_size], 24
        mov dword [multiboot2_info_instance + multiboot2_info.entry_size], 24
        mov dword [multiboot2_info_instance + multiboot2_info.entry_version], 0

        mov word di, memory_map_entries

        call get_memory_map_from_bios

        mov dword [multiboot2_info_instance + multiboot2_info.mem_map_entry_list_base_addr], memory_map_entries
        mov dword [multiboot2_info_instance + multiboot2_info.mem_map_num_entries], ebp

    fill_terminating_tag_type:
        mov dword [multiboot2_info_instance + multiboot2_info.terminating_tag_type], 0
        mov dword [multiboot2_info_instance + multiboot2_info.terminating_tag_size], 8

    mov bx, BEFORE_FINAL_RET
    call print_string

    ; Now we're ready to initialize PM.
    jmp switch_to_pm

%include "boot/memory_size.asm"
%include "boot/utils/print_string_rm.asm"
%include "boot/utils/print_hex_rm.asm"

BEFORE_NUM_KB db "Before number of KBs.", 0
AFTER_NUM_KB db "After number of KBs.", 0
BEFORE_FINAL_RET db "Before final RET.", 0
AFTER_MEM_MAP_BIOS db "After BIOS memory map.", 0

; ---------------------------------------------------------------------------- ;

; This allocates a basic GDT.
%include "boot/basic_gdt.asm"

; Switch to Protected Mode.
switch_to_pm:

    mov bx, MSG_SWITCHING_TO_PM
    call print_string

    ; We must switch off hardware interrupts until we have
    ; set-up the PM IDT, otherwise the PIT will
    ; trigger a hardware interrupt.
    cli

    ; Load our basic GDT.
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

MSG_SWITCHING_TO_PM    db "Switching to PM.", 0
BEFORE_PM_JMP          db "Before PM Jump.", 0
BEFORE_GDT             db "Before GDT.", 0
AFTER_GDT              db "After GDT.", 0

; ---------------------------------------------------------------------------- ;

[bits 32]

init_pm:

    ; Now that we loaded our GDT, the old values
    ; of the segment registers are meaningless.
    ; We point all of our segment registers to
    ; the data selector we defined in our GDT.
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

    jmp begin_pm

    jmp $                           ; We should never reach here, but we should
                                    ; hang if so.

; ---------------------------------------------------------------------------- ;

begin_pm:

    mov bx, LANDED_IN_PM
    call print_string_pm

    [extern _start]                     ; Declare that we will be referencing the external symbol ’start’,
                                        ; so the linker can substitute the final address

    push multiboot2_info_instance
    call _start                         ; invoke start() in our C kernel

    jmp $                               ; Hang forever when we return from the kernel
                                        ; (which we shouldn't).

%include "boot/utils/print_string_pm.asm"
LANDED_IN_PM db "Landed in PM.", 0
