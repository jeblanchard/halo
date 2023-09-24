set(OS_IMAGE_SOURCE_FILES
    ${OS_IMAGE_SOURCE_FILES}
    gdt.c
    start.c
)

add_subdirectory(cmd-line)
add_subdirectory(drivers)
add_subdirectory(interrupts)
add_subdirectory(networking)
add_subdirectory(physical-memory-management)
add_subdirectory(utils)
