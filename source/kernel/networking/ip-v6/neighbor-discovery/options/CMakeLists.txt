set(C_SOURCES
    mtu.c
    prefix_information.c
    redirected_header.c
    source_link_layer_address.c)

include(GetAbsolutePaths)
get_absolute_paths("${C_SOURCES}" C_SOURCES_ABS)

set(KERNEL_FRAGMENTS_C_SOURCES
    ${KERNEL_FRAGMENTS_C_SOURCES}
    ${C_SOURCES_ABS}
    PARENT_SCOPE
)

set(KERNEL_FRAGMENTS_ASM_SOURCES
    ${KERNEL_FRAGMENTS_ASM_SOURCES}
    PARENT_SCOPE
)
