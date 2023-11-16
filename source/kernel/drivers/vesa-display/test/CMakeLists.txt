set(TEST_LINK_OPTIONS -Wl,--wrap=set_byte_in_video_buffer,--wrap=get_byte_in_video_buffer)
set(TEST_LINK_OPTIONS ${TEST_LINK_OPTIONS},--wrap=get_cursor_offset,--wrap=set_cursor)
set(TEST_LINK_OPTIONS ${TEST_LINK_OPTIONS},--wrap=port_byte_out,--wrap=port_byte_in)
set(TEST_LINK_OPTIONS ${TEST_LINK_OPTIONS},--wrap=get_screen_offset)

add_cmocka_test(vesa_display
    SOURCES vesa_display_test.c ../../../utils/memory.c
        ../../../utils/standard.c
        ../../../utils/string.c ../../pit.c ../../8259a-pic/pic.c
        ../../../interrupts/idt.c
        ../../../interrupts/predefined/predefined_interrupt_loading.c
        ../../../utils/errors.c
        ../../../interrupts/predefined/ia_32_core_handlers.c
        ../../../interrupts/predefined/interrupt_handlers.asm
        ../vesa_display.c

    COMPILE_OPTIONS -Werror -Wextra -Wall
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
    LINK_OPTIONS ${TEST_LINK_OPTIONS}
)

target_include_directories(vesa_display
    PUBLIC ${CMOCKA_INCLUDE_DIR})
