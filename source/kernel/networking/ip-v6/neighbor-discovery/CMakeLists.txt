add_subdirectory(options)

set(C_SOURCES
    neighbor_solicitation.c
    router_solicitation.c)

include(GetAbsolutePaths)

get_absolute_paths("${C_SOURCES}" C_SOURCES_ABS)

set(KERNEL_FRAGMENTS_C_SOURCES
    ${KERNEL_FRAGMENTS_C_SOURCES}
    PARENT_SCOPE
)

set(KERNEL_FRAGMENTS_ASM_SOURCES
    ${KERNEL_FRAGMENTS_ASM_SOURCES}
    PARENT_SCOPE
)
