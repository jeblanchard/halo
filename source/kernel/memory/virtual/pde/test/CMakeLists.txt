add_cmocka_test(pde
    SOURCES pde_test.c ${SOURCE_DIR}/kernel/memory/virtual/pde/pde.c
    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)

add_cmocka_test(get_pt_base_address
    SOURCES get_pt_base_addr_test.c
        ${SOURCE_DIR}/kernel/memory/virtual/pde/get_pt_base_addr.c
    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)
