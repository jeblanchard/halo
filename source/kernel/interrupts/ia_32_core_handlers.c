#include "../utils/errors.h"

void handle_divide_error() {
    char msg[] = "Divide error.";
    halt_and_display_error_msg(msg);
}

void handle_debug() {
    char msg[] = "Debug.";
    halt_and_display_error_msg(msg);
}

void handle_nmi_interrupt() {
    char msg[] = "NMI Interrupt.";
    halt_and_display_error_msg(msg);
}

void handle_breakpoint() {
    char msg[] = "Breakpoint.";
    halt_and_display_error_msg(msg);
}

void handle_overflow() {
    char msg[] = "Overflow.";
    halt_and_display_error_msg(msg);
}

void handle_exceeded_bound_range() {
    char msg[] = "BOUND Range Exceeded.";
    halt_and_display_error_msg(msg);
}

void handle_invalid_opcode() {
    char msg[] = "Invalid Opcode (Undefined Opcode).";
    halt_and_display_error_msg(msg);
}

void handle_device_not_available() {
    char msg[] = "Device Not Available (No Math Coprocessor).";
    halt_and_display_error_msg(msg);
}

void handle_double_fault() {
    char msg[] = "Double Fault.";
    halt_and_display_error_msg(msg);
}

void handle_coprocessor_segment_overrun() {
    char msg[] = "CoProcessor Segment Overrun.";
    halt_and_display_error_msg(msg);
}

void handle_invalid_tss() {
    char msg[] = "Invalid TSS.";
    halt_and_display_error_msg(msg);
}

void handle_segment_not_present() {
    char msg[] = "Segment Not Present.";
    halt_and_display_error_msg(msg);
}

void handle_stack_segment_fault() {
    char msg[] = "Stack Segment Fault.";
    halt_and_display_error_msg(msg);
}

void handle_general_protection_fault() {
    char msg[] = "General Protection Fault.";
    halt_and_display_error_msg(msg);
}

void handle_page_fault() {
    char msg[] = "Page Fault.";
    halt_and_display_error_msg(msg);
}

void handle_reserved_vector_15() {
    char msg[] = "Reserved vector 15.";
    halt_and_display_error_msg(msg);
}

void handle_floating_point_error() {
    char msg[] = "Floating-Point Error (Math Fault).";
    halt_and_display_error_msg(msg);
}

void handle_alignment_check() {
    char msg[] = "Alignment Check.";
    halt_and_display_error_msg(msg);
}

void handle_machine_check() {
    char msg[] = "Machine Check.";
    halt_and_display_error_msg(msg);
}

void handle_simd_floating_point_exception() {
    char msg[] = "SIMD Floating-Point Exception.";
    halt_and_display_error_msg(msg);
}

void handle_virtualization_exception() {
    char msg[] = "Virtualization Exception.";
    halt_and_display_error_msg(msg);
}

void handle_control_protection_exception() {
    char msg[] = "Control Protection Exception.";
    halt_and_display_error_msg(msg);
}

void handle_reserved_vectors_22_thru_31() {
    char msg[] = "Reserved vector 22 through 31.";
    halt_and_display_error_msg(msg);
}

void default_handler() {
    char msg[] = "Default handler was triggered.";
    halt_and_display_error_msg(msg);
}
