#include "ia_32_core_handlers.h"
#include "vector_handler_external_declarations.h"
#include "idt.h"

static unsigned int core_handler_addresses[MAX_IR_NUM + 1];

unsigned int get_core_vector_handler_address(unsigned short vector) {
    return core_handler_addresses[vector];
}

void store_all_predefined_handlers() {
    core_handler_addresses[0] = (unsigned int) handle_divide_error;
    core_handler_addresses[1] = (unsigned int) handle_debug;
    core_handler_addresses[2] = (unsigned int) handle_nmi_interrupt;
    core_handler_addresses[3] = (unsigned int) handle_breakpoint;
    core_handler_addresses[4] = (unsigned int) handle_overflow;
    core_handler_addresses[5] = (unsigned int) handle_exceeded_bound_range;
    core_handler_addresses[6] = (unsigned int) handle_invalid_opcode;
    core_handler_addresses[7] = (unsigned int) handle_device_not_available;
    core_handler_addresses[8] = (unsigned int) handle_double_fault;
    core_handler_addresses[9] = (unsigned int) handle_coprocessor_segment_overrun;
    core_handler_addresses[10] = (unsigned int) handle_invalid_tss;
    core_handler_addresses[11] = (unsigned int) handle_segment_not_present;
    core_handler_addresses[12] = (unsigned int) handle_stack_segment_fault;
    core_handler_addresses[13] = (unsigned int) handle_general_protection_fault;
    core_handler_addresses[14] = (unsigned int) handle_page_fault;
    core_handler_addresses[15] = (unsigned int) handle_reserved_vector_15;
    core_handler_addresses[16] = (unsigned int) handle_floating_point_error;
    core_handler_addresses[17] = (unsigned int) handle_alignment_check;
    core_handler_addresses[18] = (unsigned int) handle_machine_check;
    core_handler_addresses[19] = (unsigned int) handle_simd_floating_point_exception;
    core_handler_addresses[20] = (unsigned int) handle_virtualization_exception;
    core_handler_addresses[21] = (unsigned int) handle_control_protection_exception;
    core_handler_addresses[22] = (unsigned int) handle_reserved_vectors_22_thru_31;

    for (int i = 32; i <= MAX_IR_NUM; i++) {
        core_handler_addresses[i] = (unsigned int) default_handler;
    }
}

void load_initial_handlers_to_idt() {

    store_all_predefined_handlers();

    install_ir(0, handle_vector_0);
    install_ir(1, handle_vector_1);
    install_ir(2, handle_vector_2);
    install_ir(3, handle_vector_3);
    install_ir(4, handle_vector_4);
    install_ir(5, handle_vector_5);
    install_ir(6, handle_vector_6);
    install_ir(7, handle_vector_7);
    install_ir(8, handle_vector_8);
    install_ir(9, handle_vector_9);
    install_ir(10, handle_vector_10);
    install_ir(11, handle_vector_11);
    install_ir(12, handle_vector_12);
    install_ir(13, handle_vector_13);
    install_ir(14, handle_vector_14);
    install_ir(15, handle_vector_15);
    install_ir(16, handle_vector_16);
    install_ir(17, handle_vector_17);
    install_ir(18, handle_vector_18);
    install_ir(19, handle_vector_19);
    install_ir(20, handle_vector_20);
    install_ir(21, handle_vector_21);
    install_ir(22, handle_vector_22);
    install_ir(23, handle_vector_23);
    install_ir(24, handle_vector_24);
    install_ir(25, handle_vector_25);
    install_ir(26, handle_vector_26);
    install_ir(27, handle_vector_27);
    install_ir(28, handle_vector_28);
    install_ir(29, handle_vector_29);
    install_ir(30, handle_vector_30);
    install_ir(31, handle_vector_31);
    install_ir(32, handle_vector_32);
    install_ir(33, handle_vector_33);
    install_ir(34, handle_vector_34);
    install_ir(35, handle_vector_35);
    install_ir(36, handle_vector_36);
    install_ir(37, handle_vector_37);
    install_ir(38, handle_vector_38);
    install_ir(39, handle_vector_39);
    install_ir(40, handle_vector_40);
    install_ir(41, handle_vector_41);
    install_ir(42, handle_vector_42);
    install_ir(43, handle_vector_43);
    install_ir(44, handle_vector_44);
    install_ir(45, handle_vector_45);
    install_ir(46, handle_vector_46);
    install_ir(47, handle_vector_47);
    install_ir(48, handle_vector_48);
    install_ir(49, handle_vector_49);
    install_ir(50, handle_vector_50);
}
