add_cmocka_test(process_test
    SOURCES process_test.c ${SOURCE_DIR}/kernel/process.c

    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)
