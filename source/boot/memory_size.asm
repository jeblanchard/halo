[bits 16]

; Returns:
;     ecx = number of KBs in system
get_num_kb_in_system:
    push eax
    push ebx

    mov bx, BEFORE_MEMORY_SIZE
    call print_string

    call get_memory_size_info_from_bios

    mov bx, AFTER_MEMORY_SIZE
    call print_string

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

	mov bx, AFTER_INT_15
    call print_string

	jc .error
	cmp	ah, 0x86	    ; Unsupported function

	je .error
	cmp	ah, 0x80	    ; Invalid command

	je .error           ; BIOS may have stored the result in ax and bx, or cx and dx.
	jcxz .use_ax

	mov	ax, cx
	mov	bx, dx

	mov bx, BEFORE_RET
    call print_string

    pop edx
    pop ecx

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

    mov bx, BEG_MEM_MAP
    call print_string

	xor	ebp, ebp			    ; number of entries will be stored here

	mov	eax, 0x0000e820                    ; select GET SYSTEM MEMORY MAP instruction
	mov	edx, 0x534d4150	                   ; 'SMAP'
	mov	ecx, memory_map_entry_size		   ; memory map entry struct is 24 bytes

;	mov bx, BEFORE_MEM_MAP_INT
;    call print_string

    xor	ebx, ebx                           ; set ebx to 0 to start at the beginning of the map
	int	0x15			    ; get first entry
	jc .error

;	mov bx, AFTER_MEM_MAP_INT
;    call print_string

	cmp	eax, 0x534d4150     ; BIOS returns SMAP in eax
	jne	.error

	test ebx, ebx		    ; if ebx is still 0, our memory map has only one entry
	je .error

;    mov bx, BEFORE_START
;    call print_string

	jmp	.start

    .next_entry:
        mov	edx, 0x534d4150		         ; some BIOSs trash this register
        mov	ecx, memory_map_entry_size
        mov	eax, 0x0000e820

;        mov bx, BEF_NEXT_ENTRY_INT
;        call print_string

        int	0x15			    ; get next entry (ebx has already been set by the
                                ; previous call)

;        mov bx, AFTER_NEXT_ENTRY_INT
;        call print_string

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

        pusha
        mov bx, GOOD_ENTRY
        call print_string
        popa

    .skip_entry:
        cmp	ebx, 0			; if ebx return is 0, list is done
        jne	.next_entry		; if ebx is non-zero, we go to the next entry

        jmp	.done

    .error:
        stc

    .done:

        pusha
        mov bx, DONE
        call print_string
        popa

        ret

BEFORE_MEMORY_SIZE db "Before memory size.", 0
AFTER_MEMORY_SIZE db "After memory size.", 0
AFTER_INT_15 db "After INT 15.", 0
BEFORE_RET db "Before RET.", 0
BEG_MEM_MAP db "Beginning of memory map.", 0
BEFORE_MEM_MAP_INT db "Before mem. map. INT.", 0
AFTER_MEM_MAP_INT db "After mem. map INT.", 0
DONE db "Done getting memory map.", 0
BEFORE_START db "Before start label.", 0
GOOD_ENTRY db "Good entry.", 0
BEF_NEXT_ENTRY_INT db "Before next entry INT.", 0
AFTER_NEXT_ENTRY_INT db "After next entry INT.", 0
