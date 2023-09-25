set(SOURCE_FILES
    neighbor_solicitation.c
    router_solicitation.c)

include(GetAbsolutePaths)
get_absolute_paths("${SOURCE_FILES}" ABSOLUTE_PATHS)

add_subdirectory(options)

set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE
)
