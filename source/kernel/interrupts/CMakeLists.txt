set(SOURCE_FILES
    idt.c)

include(GetAbsolutePaths)
get_absolute_paths("${SOURCE_FILES}" ABSOLUTE_PATHS)

add_subdirectory(predefined)

set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE
)
