#include "ia_32_core_handlers.h"
#include "vector_handler_external_declarations.h"
#include "idt.h"

static unsigned int core_handler_addresses[MAX_IR_NUM + 1];

void assign_core_handler_address(unsigned short ir_vector, void* handler) {
    core_handler_addresses[ir_vector] = (unsigned int) handler;
}

unsigned int get_core_vector_handler_address(unsigned short vector) {
    return core_handler_addresses[vector];
}

void store_all_predefined_core_handlers() {
    assign_core_handler_address(0, handle_divide_error);
    assign_core_handler_address(1, handle_debug);
    assign_core_handler_address(2, handle_nmi_interrupt);
    assign_core_handler_address(3, handle_breakpoint);
    assign_core_handler_address(4, handle_overflow);
    assign_core_handler_address(5, handle_exceeded_bound_range);
    assign_core_handler_address(6, handle_invalid_opcode);
    assign_core_handler_address(7, handle_device_not_available);
    assign_core_handler_address(8, handle_double_fault);
    assign_core_handler_address(9, handle_coprocessor_segment_overrun);
    assign_core_handler_address(10, handle_invalid_tss);
    assign_core_handler_address(11, handle_segment_not_present);
    assign_core_handler_address(12, handle_general_protection_fault);
    assign_core_handler_address(13, handle_general_protection_fault);
    assign_core_handler_address(14, handle_page_fault);
    assign_core_handler_address(15, handle_reserved_vector_15);
    assign_core_handler_address(16, handle_floating_point_error);
    assign_core_handler_address(17, handle_alignment_check);
    assign_core_handler_address(18, handle_machine_check);
    assign_core_handler_address(19, handle_simd_floating_point_exception);
    assign_core_handler_address(20, handle_virtualization_exception);
    assign_core_handler_address(21, handle_control_protection_exception);

    for (int i = 22; i <= 31; i++) {
        assign_core_handler_address(i, handle_reserved_vectors_22_thru_31);
    }


    for (int i = 32; i <= MAX_IR_NUM; i++) {
        assign_core_handler_address(i, default_handler);
    }
}

void load_initial_handlers_to_idt() {

    store_all_predefined_core_handlers();

    add_handler_to_idt(0, handle_vector_0);
    add_handler_to_idt(1, handle_vector_1);
    add_handler_to_idt(2, handle_vector_2);
    add_handler_to_idt(3, handle_vector_3);
    add_handler_to_idt(4, handle_vector_4);
    add_handler_to_idt(5, handle_vector_5);
    add_handler_to_idt(6, handle_vector_6);
    add_handler_to_idt(7, handle_vector_7);
    add_handler_to_idt(8, handle_vector_8);
    add_handler_to_idt(9, handle_vector_9);
    add_handler_to_idt(10, handle_vector_10);
    add_handler_to_idt(11, handle_vector_11);
    add_handler_to_idt(12, handle_vector_12);
    add_handler_to_idt(13, handle_vector_13);
    add_handler_to_idt(14, handle_vector_14);
    add_handler_to_idt(15, handle_vector_15);
    add_handler_to_idt(16, handle_vector_16);
    add_handler_to_idt(17, handle_vector_17);
    add_handler_to_idt(18, handle_vector_18);
    add_handler_to_idt(19, handle_vector_19);
    add_handler_to_idt(20, handle_vector_20);
    add_handler_to_idt(21, handle_vector_21);
    add_handler_to_idt(22, handle_vector_22);
    add_handler_to_idt(23, handle_vector_23);
    add_handler_to_idt(24, handle_vector_24);
    add_handler_to_idt(25, handle_vector_25);
    add_handler_to_idt(26, handle_vector_26);
    add_handler_to_idt(27, handle_vector_27);
    add_handler_to_idt(28, handle_vector_28);
    add_handler_to_idt(29, handle_vector_29);
    add_handler_to_idt(30, handle_vector_30);
    add_handler_to_idt(31, handle_vector_31);
    add_handler_to_idt(32, handle_vector_32);
    add_handler_to_idt(33, handle_vector_33);
    add_handler_to_idt(34, handle_vector_34);
    add_handler_to_idt(35, handle_vector_35);
    add_handler_to_idt(36, handle_vector_36);
    add_handler_to_idt(37, handle_vector_37);
    add_handler_to_idt(38, handle_vector_38);
    add_handler_to_idt(39, handle_vector_39);
    add_handler_to_idt(40, handle_vector_40);
    add_handler_to_idt(41, handle_vector_41);
    add_handler_to_idt(42, handle_vector_42);
    add_handler_to_idt(43, handle_vector_43);
    add_handler_to_idt(44, handle_vector_44);
    add_handler_to_idt(45, handle_vector_45);
    add_handler_to_idt(46, handle_vector_46);
    add_handler_to_idt(47, handle_vector_47);
    add_handler_to_idt(48, handle_vector_48);
    add_handler_to_idt(49, handle_vector_49);
    add_handler_to_idt(50, handle_vector_50);
}
