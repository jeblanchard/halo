add_cmocka_test(ns16550a_uart_test
    SOURCES message_test.c ${SOURCE_DIR}/kernel/utils/memory.c
    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)

target_include_directories(ns16550a_uart_test
    PUBLIC ${CMOCKA_INCLUDE_DIR})
