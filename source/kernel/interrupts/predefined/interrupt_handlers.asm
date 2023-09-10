[bits 32]

%include "source/boot/utils/print_string_pm.asm"

extern _get_core_vector_handler_address

; Gets the address of the core handler C function
; to call.
;
; Input:
;     ax = Interrupt Vector
;
; Returns:
;     eax = 32 bit address of C core handler
get_handler_address:
    push ax                                   ; push vector on the stack
    call _get_core_vector_handler_address     ; get address of core handler
    add esp, byte 2                           ; remove parameter from the stack

    ret                                       ; eax holds the C function address

global _handle_vector_0

_handle_vector_0:

    mov ax, 0
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_1

_handle_vector_1:

    mov ax, 1
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_2

_handle_vector_2:

    mov ax, 2
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_3

_handle_vector_3:

    mov ax, 3
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_4

_handle_vector_4:

    mov ax, 4
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_5

_handle_vector_5:

    mov ax, 5
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_6

_handle_vector_6:

    mov ebx, GOT_HERE_MSG
    call print_string_pm

    mov ax, 6
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

GOT_HERE_MSG db "Got to beginning of vector 6.", 0

global _handle_vector_7

_handle_vector_7:

    mov ax, 7
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_8

_handle_vector_8:

    mov ax, 8
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_9

_handle_vector_9:

    mov ax, 9
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_10

_handle_vector_10:

    mov ax, 10
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_11

_handle_vector_11:

    mov ax, 11
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_12

_handle_vector_12:

    mov ax, 12
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_13

_handle_vector_13:

    mov ax, 13
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_14

_handle_vector_14:

    mov ax, 14
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_15

_handle_vector_15:

    mov ax, 15
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_16

_handle_vector_16:

    mov ax, 16
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_17

_handle_vector_17:

    mov ax, 17
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_18

_handle_vector_18:

    mov ax, 18
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_19

_handle_vector_19:

    mov ax, 19
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_20

_handle_vector_20:

    mov ax, 20
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_21

_handle_vector_21:

    mov ax, 21
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_22

_handle_vector_22:

    mov ax, 22
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_23

_handle_vector_23:

    mov ax, 23
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_24

_handle_vector_24:

    mov ax, 24
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_25

_handle_vector_25:

    mov ax, 25
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_26

_handle_vector_26:

    mov ax, 26
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_27

_handle_vector_27:

    mov ax, 27
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_28

_handle_vector_28:

    mov ax, 28
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_29

_handle_vector_29:

    mov ax, 29
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_30

_handle_vector_30:

    mov ax, 30
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_31

_handle_vector_31:

    mov ax, 31
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_32

_handle_vector_32:

    mov ax, 32
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_33

_handle_vector_33:

    mov ax, 33
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_34

_handle_vector_34:

    mov ax, 34
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_35

_handle_vector_35:

    mov ax, 35
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_36

_handle_vector_36:

    mov ax, 36
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_37

_handle_vector_37:

    mov ax, 37
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_38

_handle_vector_38:

    mov ax, 38
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_39

_handle_vector_39:

    mov ax, 39
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_40

_handle_vector_40:

    mov ax, 40
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_41

_handle_vector_41:

    mov ax, 41
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_42

_handle_vector_42:

    mov ax, 42
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_43

_handle_vector_43:

    mov ax, 43
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_44

_handle_vector_44:

    mov ax, 44
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_45

_handle_vector_45:

    mov ax, 45
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_46

_handle_vector_46:

    mov ax, 46
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_47

_handle_vector_47:

    mov ax, 47
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_48

_handle_vector_48:

    mov ax, 48
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_49

_handle_vector_49:

    mov ax, 49
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret

global _handle_vector_50

_handle_vector_50:

    mov ax, 50
    call get_handler_address

    call eax                                  ; call handler that was provided

    iret
