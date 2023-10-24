add_cmocka_test(ip_v6_address_test
    SOURCES addressing_test.c ../addressing.c
    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)

target_include_directories(ip_v6_address_test
    PUBLIC ${CMOCKA_INCLUDE_DIR})
