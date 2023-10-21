add_cmocka_test(ns16550a_uart_test
    SOURCES message_test.c ../../../utils/memory.c
    COMPILE_OPTIONS -Werror -Wextra -Wall
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)

target_include_directories(ns16550a_uart_test
    PUBLIC ${CMOCKA_INCLUDE_DIR})
