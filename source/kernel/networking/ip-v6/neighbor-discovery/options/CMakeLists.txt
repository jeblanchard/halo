set(SOURCE_FILES
    base.c
    mtu.c
    prefix_information.c
    redirected_header.c
    source_link_layer_address.c)

include(GetAbsolutePaths)
get_absolute_paths("${SOURCE_FILES}" ABSOLUTE_PATHS)

set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE
)
