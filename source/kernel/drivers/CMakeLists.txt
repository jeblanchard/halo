add_subdirectory(8259a-pic)
add_subdirectory(dp8390-nic)
add_subdirectory(ns16550a-uart)

set(SOURCE_FILES
    keyboard.c
    pci.c
    vesa_display.c
    pit.c)

include(GetAbsolutePaths)
get_absolute_paths("${SOURCE_FILES}" ABSOLUTE_PATHS)

set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE
)
