; Ensures that we jump straight into the kernel’s entry function.

[bits 32]

struc multiboot2_info
    .total_size             resd    1
    .reserved               resd    1

    .mem_info_type          resd    1
    .mem_info_size          resd    1
	.num_kb_in_mem	        resd	1

	.mem_map_info_type      resd    1
	.mem_map_info_size      resd    1
	.entry_size             resd    1
	.entry_version          resd    1
	.mem_map_base_addr      resd    1
	.mem_map_num_entries    resd    1

	.terminating_tag_type   resd    1
	.terminating_tag_size   resd    1
endstruc

multiboot2_info_instance: resb multiboot2_info_size

fill_multiboot2_fixed_part:
    mov dword [multiboot2_info_instance + multiboot2_info.total_size], multiboot2_info_size
    mov dword [multiboot2_info_instance + multiboot2_info.reserved], 0

fill_multiboot2_mem_info:
    mov dword [multiboot2_info_instance + multiboot2_info.mem_info_type], 4
    mov dword [multiboot2_info_instance + multiboot2_info.mem_info_size], 16

    call get_num_kb_in_system
    mov dword [multiboot2_info_instance + multiboot2_info.num_kb_in_mem], ecx

fill_multiboot2_mem_map_info:
    mov dword [multiboot2_info_instance + multiboot2_info.mem_map_info_type], 6
    mov dword [multiboot2_info_instance + multiboot2_info.mem_map_info_size], 24
    mov dword [multiboot2_info_instance + multiboot2_info.entry_size], 24
    mov dword [multiboot2_info_instance + multiboot2_info.entry_version], 0

    call get_memory_map_from_bios

    mov eax, es:di
    mov dword [multiboot2_info_instance + multiboot2_info.mem_map_base_addr], eax
    mov dword [multiboot2_info_instance + multiboot2_info.mem_map_num_entries], ebp

fill_terminating_tag_type:
    mov dword [multiboot2_info_instance + multiboot2_info.terminating_tag_type], 0
    mov dword [multiboot2_info_instance + multiboot2_info.terminating_tag_size], 8

; Returns:
;     ecx = number of KBs in system
get_num_kb_in_system:
    push eax
    push ebx

    call get_memory_size_info_from_bios

    xor ecx, ecx          ; clear ecx, this is where we will
                          ; store our result

    add eax, 1024         ; number of KBs from 0MB to 16MB
    add ecx, eax

    xor eax, eax          ; clear eax in order to use the mul
                          ; instruction

    mov eax, ebx          ; move ebx (number of 64KB blocks above 16MB)
                          ; to eax in order to use the multiply
                          ; instruction

    mov ebx, 64           ; we're done with ebx, so let's fill it with
                          ; our multiplication factor (64)

    mul ebx               ; eax now holds the number of KBs above 16MB

    add ecx, eax          ; ecx now holds the number of KBs in the system

    pop eax
    pop ebx
    ret

; Returns:
;    eax = KB between 1MB and 16MB
;        = -1 on error
;    ebx = number of 64KB blocks above 16MB
;    ebx = 0
get_memory_size_info_from_bios:
	push ecx
	push edx

	xor	ecx, ecx	    ; Clear all registers. This is needed for testing later.
	xor	edx, edx

	mov	ax, 0xe801
	int	0x15

	jc .error
	cmp	ah, 0x86	    ; Unsupported function

	je .error
	cmp	ah, 0x80	    ; Invalid command

	je .error           ; BIOS may have stored the result in ax and bx, or cx and dx.
	jcxz .use_ax

	mov	ax, cx
	mov	bx, dx

	ret

    ; Memory size is in ax and bx already, return it.
    .use_ax:
        pop	edx
        pop	ecx
        ret

    .error:
        mov	ax, -1
        mov	bx, 0
        pop	edx
        pop	ecx
        ret

struc memory_map_entry
	.base_addr	    resq	1   ; Base address of address range.
	.length		    resq	1	; Length of address range in bytes.
	.type		    resd	1	; Type of address range.
	.reserved	    resd	1	; Reserved.
endstruc

; Get memory map from BIOS
;
; Returns:
;     BP = entry count
;     ES:DI = base pointer of map
get_memory_map_from_bios:
	pusha

	xor	ebp, ebp			    ; number of entries will be stored here

	mov	eax, 0xe820                        ; select GET SYSTEM MEMORY MAP instruction
	mov	edx, 0x534d4150	                   ; 'SMAP'
	xor	ebx, ebx                           ; set ebx to 0 to start at the beginning of the map
	mov	ecx, memory_map_entry_size		   ; memory map entry struct is 24 bytes

	int	0x15			    ; get first entry
	jc .error

	cmp	eax, 0x534d4150     ; BIOS returns SMAP in eax
	jne	.error

	test ebx, ebx		    ; if ebx didn't change, our memory map has only one entry
	je .error

	jmp	.start

    .next_entry:
        mov	edx, 0x534d4150		; some BIOSs trash this register
        mov	ecx, 24			    ; entry is 24 bytes
        mov	eax, 0xe820
        int	0x15			    ; get next entry (ebx has already been set by the
                                ; previous call)

    .start:
        jcxz .skip_entry		; if actual length is bytes is 0, skip entry

    .no_text:
        mov	ecx, [es:di + memory_map_entry.length]	      ; get lower 32 bits of length
        test ecx, ecx		                              ; if lower 32 bits are non-zero we know
                                                          ; this is a valid entry

        jne	short .good_entry

        mov	ecx, [es:di + memory_map_entry.length + 4]    ; get upper 32 bits of length

        jecxz .skip_entry		                          ; if upper 32 bits are also 0 (meaning that
                                                          ; the entry length is 0) we go to the next
                                                          ; entry

    .good_entry:
        inc	ebp			    ; increment entry count
        add	di, 24			; point di to next entry in buffer

    .skip_entry:
        cmp	ebx, 0			; if ebx return is 0, list is done
        jne	.next_entry		; if ebx is non-zero, we go to the next entry

        jmp	.done

    .error:
        stc

    .done:
        popad
        ret

; ------------------------------------------------------------------------------------------------- ;

[extern _start]         ; Declare that we will be referencing the external symbol ’start’,
                        ; so the linker can substitute the final address

push multiboot2_info_size
call _start             ; invoke start() in our C kernel
jmp $                   ; Hang forever when we return from the kernel
