set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    gdt.c
    start.c
    PARENT_SCOPE
)

add_subdirectory(cmd-line)
add_subdirectory(drivers)
add_subdirectory(interrupts)
add_subdirectory(networking)
add_subdirectory(physical-memory-management)
add_subdirectory(utils)
