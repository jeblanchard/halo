add_cmocka_test(physical_mem
    SOURCES physical_mem_test.c ${SOURCE_DIR}/kernel/utils/memory.c
        ${SOURCE_DIR}/kernel/utils/errors.c
        ${SOURCE_DIR}/kernel/drivers/vesa-display/vesa_display.c
        ${SOURCE_DIR}/kernel/drivers/vesa-display/video_buffer.c
        ${SOURCE_DIR}/kernel/utils/io.c
        ${SOURCE_DIR}/kernel/utils/string.c
        ${SOURCE_DIR}/kernel/drivers/pit.c
        ${SOURCE_DIR}/kernel/drivers/8259a-pic/pic.c
        ${SOURCE_DIR}/kernel/interrupts/idt.c
        ${SOURCE_DIR}/kernel/interrupts/predefined/predefined_interrupt_loading.c
        ${SOURCE_DIR}/kernel/interrupts/predefined/interrupt_handlers.asm
        ${SOURCE_DIR}/kernel/interrupts/predefined/ia_32_core_handlers.c

    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)


set(TEST_LINK_OPTIONS -Wl,--wrap=alloc_block)

add_cmocka_test(mem_manager
    SOURCES manager_test.c

    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
    LINK_OPTIONS ${TEST_LINK_OPTIONS}
)