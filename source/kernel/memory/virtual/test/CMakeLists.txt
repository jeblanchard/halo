add_cmocka_test(pte
    SOURCES pte_test.c ${SOURCE_DIR}/kernel/memory/virtual/pte.c
    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
)

set(TEST_LINK_OPTIONS -Wl,--wrap=alloc_block,--wrap=free_block,--wrap=load_pdbr_asm)
set(TEST_LINK_OPTIONS ${TEST_LINK_OPTIONS},--wrap=phys_mem_needs_config,--wrap=frame_has_mem_mapped_io)
set(TEST_LINK_OPTIONS ${TEST_LINK_OPTIONS},--wrap=alloc_spec_frame)

add_cmocka_test(virtual_mem_manager
    SOURCES virtual_mem_mgr_test.c ${SOURCE_DIR}/kernel/memory/virtual/virtual_mem_mgr.c
        ${SOURCE_DIR}/kernel/memory/virtual/pte.c ${SOURCE_DIR}/kernel/memory/virtual/pde/pde.c
        ${SOURCE_DIR}/kernel/utils/memory.c
        ${SOURCE_DIR}/boot/kernel_phys_addr.asm
    COMPILE_OPTIONS ${DEFAULT_GCC_OPTIONS} ${CMOCKA_GCC_INCLUDE_OPTION}
    LINK_LIBRARIES ${CMOCKA_LIBRARY}
    LINK_OPTIONS ${TEST_LINK_OPTIONS}
)
