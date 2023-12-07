add_cmocka_test(pde
    SOURCES pde_test.c ${SOURCE_DIR}/kernel/memory/virtual/pde/pde.c
        ${SOURCE_DIR}/kernel/memory/virtual/pte.c
    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)
