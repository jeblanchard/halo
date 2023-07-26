;---------------------------------------------
;	Get memory size for >64M configurations
;	ret\ ax=KB between 1MB and 16MB
;	ret\ bx=number of 64K blocks above 16MB
;	ret\ bx=0 and ax= -1 on error
;---------------------------------------------

BiosGetMemorySize64MB:
	push ecx
	push edx
	xor	ecx, ecx	    ; clear all registers. This is needed for testing later
	xor	edx, edx
	mov	ax, 0xe801
	int	0x15
	jc .error
	cmp	ah, 0x86	    ; unsupported function
	je .error
	cmp	ah, 0x80	    ; invalid command
	je .error
	jcxz .use_ax	    ; BIOS may have stored it in ax,bx or cx,dx. test if cx is 0
	mov	ax, cx	        ; its not, so it should contain mem size; store it
	mov	bx, dx

.use_ax:
	pop	edx			    ; mem size is in ax and bx already, return it
	pop	ecx
	ret

.error:
	mov	ax, -1
	mov	bx, 0
	pop	edx
	pop	ecx
	ret
