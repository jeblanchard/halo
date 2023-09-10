void handle_divide_error();

void handle_debug();

void handle_nmi_interrupt();

void handle_breakpoint();

void handle_overflow();

void handle_exceeded_bound_range();

void handle_invalid_opcode();

void handle_device_not_available();

void handle_double_fault();

void handle_coprocessor_segment_overrun();

void handle_invalid_tss();

void handle_segment_not_present();

void handle_stack_segment_fault();

void handle_general_protection_fault();

void handle_page_fault();

void handle_reserved_vector_15();

void handle_floating_point_error();

void handle_alignment_check();

void handle_machine_check();

void handle_simd_floating_point_exception();

void handle_virtualization_exception();

void handle_control_protection_exception();

void handle_reserved_vectors_22_thru_31();

void default_handler();
