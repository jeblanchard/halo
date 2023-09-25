set(SOURCE_FILES
    address.c
    functionality.c
    icmp_v6.c
    message_transmission.c
    router_discovery.c
    stateless_address_autoconfiguration.c)

include(GetAbsolutePaths)
get_absolute_paths(${SOURCE_FILES} ABSOLUTE_PATHS)

add_subdirectory(extension-headers)
add_subdirectory(neighbor-discovery)

set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE
)
