set(MEM_MGR_LINK_OPTIONS -Wl,--wrap=alloc_block)

add_halo_test(
    NAME mem_manager_test
    SOURCES manager_test.c
    LINK_OPTIONS ${MEM_MGR_LINK_OPTIONS}
)

set(PHYS_MEM_TEST_LINK_OPTIONS -Wl,--wrap=load_pdbr_asm)

add_halo_test(
    NAME physical_mem_test
    SOURCES physical_mem_test.c 
        ${SOURCE_DIR}/kernel/memory/phys_mem_mgr.c
        ${SOURCE_DIR}/kernel/utils/memory.c
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

    LINK_OPTIONS ${PHYS_MEM_TEST_LINK_OPTIONS}
)
