add_cmocka_test(pte
    SOURCES pte_test.c ${SOURCE_DIR}/kernel/memory/virtual/pte.c
    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)

add_cmocka_test(pde
    SOURCES pde_test.c ${SOURCE_DIR}/kernel/memory/virtual/pde.c
    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)

set(TEST_LINK_OPTIONS -Wl,--wrap=alloc_block,--wrap=free_block)

add_cmocka_test(virtual_mem_manager
    SOURCES virtual_mem_mgr_test.c ${SOURCE_DIR}/kernel/memory/virtual/virtual_mem_mgr.c
        ${SOURCE_DIR}/kernel/memory/virtual/pte.c
    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
    LINK_OPTIONS ${TEST_LINK_OPTIONS}
)
