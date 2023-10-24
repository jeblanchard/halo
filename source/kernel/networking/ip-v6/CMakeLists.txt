add_subdirectory(extension-headers)
add_subdirectory(neighbor-discovery)
add_subdirectory(test)

set(C_SOURCES
    addressing.c
    functionality.c
    icmp_v6.c
    message_transmission.c
    router_discovery.c
    stateless_address_autoconfiguration.c)

include(GetAbsolutePaths)

get_absolute_paths("${C_SOURCES}" C_SOURCES_ABS)

set(KERNEL_FRAGMENTS_C_SOURCES
    ${KERNEL_FRAGMENTS_C_SOURCES}
    ${C_SOURCES_ABS}
    PARENT_SCOPE
)

set(KERNEL_FRAGMENTS_ASM_SOURCES
    ${KERNEL_FRAGMENTS_ASM_SOURCES}
    ${ASM_SOURCES_ABS}
    PARENT_SCOPE
)
