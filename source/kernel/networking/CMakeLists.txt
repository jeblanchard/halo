set(SOURCE_FILES
    udp.c)

include(GetAbsolutePaths)
set(ABSOLUTE_PATHS "")
get_absolute_paths("${SOURCE_FILES}" ABSOLUTE_PATHS)

add_subdirectory(ip-v6)

set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE
)
