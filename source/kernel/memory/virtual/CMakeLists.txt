add_subdirectory(test)

set(C_SOURCES
    pte.c
    virtual_mem_mgr.c)

set(ASM_SOURCES
    load_pdbr.asm
    flush_tlb_entry.asm
)

include(GetAbsolutePaths)
get_absolute_paths("${C_SOURCES}" C_SOURCES_ABS)

set(KERNEL_FRAGMENTS_C_SOURCES
    ${KERNEL_FRAGMENTS_C_SOURCES}
    ${C_SOURCES_ABS}
    PARENT_SCOPE
)

set(KERNEL_FRAGMENTS_ASM_SOURCES
    ${KERNEL_FRAGMENTS_ASM_SOURCES}
    PARENT_SCOPE
)