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

add_cmocka_test(virtual_mem_manager
    SOURCES manager_test.c ${SOURCE_DIR}/kernel/memory/virtual/manager.c
    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)
