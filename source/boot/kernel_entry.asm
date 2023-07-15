; Ensures that we jump straight into the kernel’s entry function.
[bits 32]               ; We’re in protected mode by now, so use 32-bit instructions.
[extern _start]       ; Declare that we will be referencing the external symbol ’start’,
                      ; so the linker can substitute the final address

call _start          ; invoke start() in our C kernel
jmp $                ; Hang forever when we return from the kernel
