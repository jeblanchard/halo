add_halo_test(
    NAME pte_test
    SOURCES pte_test.c ${SOURCE_DIR}/kernel/memory/virtual/pte.c
)

set(TEST_LINK_OPTIONS -Wl,--wrap=alloc_block,--wrap=free_block,--wrap=load_pdbr_asm)
set(TEST_LINK_OPTIONS ${TEST_LINK_OPTIONS},--wrap=phys_mem_needs_config,--wrap=frame_has_mem_mapped_io)
set(TEST_LINK_OPTIONS ${TEST_LINK_OPTIONS},--wrap=alloc_spec_frame)

add_halo_test(
    NAME virtual_mem_manager_test
    SOURCES virtual_mem_mgr_test.c ${SOURCE_DIR}/kernel/memory/virtual/virtual_mem_mgr.c
        ${SOURCE_DIR}/kernel/memory/virtual/pte.c ${SOURCE_DIR}/kernel/memory/virtual/pde.c
        ${SOURCE_DIR}/kernel/utils/memory.c
        ${SOURCE_DIR}/boot/kernel_phys_addr.asm
    LINK_OPTIONS ${TEST_LINK_OPTIONS}
)

add_halo_test(
    NAME pde_test
    SOURCES pde_test.c ${SOURCE_DIR}/kernel/memory/virtual/pde.c
        ${SOURCE_DIR}/kernel/memory/virtual/pte.c
)
