add_subdirectory(drivers)
add_subdirectory(interrupts)
add_subdirectory(networking)
add_subdirectory(memory)
add_subdirectory(utils)
add_subdirectory(test)

set(C_SOURCES
    gdt.c
    start.c
    process.c)

include(GetAbsolutePaths)
get_absolute_paths("${C_SOURCES}" C_SOURCES_ABS)

set(KERNEL_FRAGMENTS_C_SOURCES
    ${KERNEL_FRAGMENTS_C_SOURCES}
    ${C_SOURCES_ABS}
    PARENT_SCOPE
)

set(KERNEL_FRAGMENTS_ASM_SOURCES
    ${KERNEL_FRAGMENTS_C_SOURCES}
    PARENT_SCOPE
)
