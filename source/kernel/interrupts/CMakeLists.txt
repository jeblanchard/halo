add_subdirectory(predefined)
add_subdirectory(test)

set(C_SOURCES
    idt.c)

include(GetAbsolutePaths)
get_absolute_paths("${C_SOURCES}" C_SOURCES_ABS)

set(KERNEL_FRAGMENTS_C_SOURCES
    ${KERNEL_FRAGMENTS_C_SOURCES}
    ${C_SOURCES_ABS}
    PARENT_SCOPE
)

set(KERNEL_FRAGMENTS_ASM_SOURCES
    PARENT_SCOPE
)
