add_subdirectory(8259a-pic)
add_subdirectory(dp8390-nic)
add_subdirectory(ns16550a-uart)

set(C_SOURCES
    keyboard.c
    pci.c
    vesa_display.c
    pit.c)

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
