add_cmocka_test(process
    SOURCES process_test.c ${SOURCE_DIR}/kernel/process/process.c

    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)
