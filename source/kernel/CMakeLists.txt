add_subdirectory(cmd-line)
add_subdirectory(drivers)
add_subdirectory(interrupts)
add_subdirectory(networking)
add_subdirectory(physical-memory-management)
add_subdirectory(utils)

set(SOURCE_FILES
    gdt.c
    start.c)

include(GetAbsolutePaths)
get_absolute_paths("${SOURCE_FILES}" ABSOLUTE_PATHS)

set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE
)
