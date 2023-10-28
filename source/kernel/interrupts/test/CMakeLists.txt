add_cmocka_test(idt_test
    SOURCES idt_test.c
        ${SOURCE_DIR}/kernel/utils/memory.c
        ${SOURCE_DIR}/kernel/interrupts/predefined/predefined_interrupt_loading.c
        ${SOURCE_DIR}/kernel/interrupts/predefined/interrupt_handlers.asm
        ${SOURCE_DIR}/kernel/interrupts/predefined/ia_32_core_handlers.c
        ${SOURCE_DIR}/kernel/utils/errors.c
        ${SOURCE_DIR}/kernel/drivers/vesa-display/vesa_display.c
        ${SOURCE_DIR}/kernel/drivers/vesa-display/video_buffer.c
        ${SOURCE_DIR}/kernel/utils/string.c
        ${SOURCE_DIR}/kernel/utils/io.c
        ${SOURCE_DIR}/kernel/drivers/pit.c
        ${SOURCE_DIR}/kernel/drivers/8259a-pic/pic.c

    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
        LINK_LIBRARIES ${CMOCKA_LIBRARY}
)
