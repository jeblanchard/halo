add_subdirectory(boot)
add_subdirectory(kernel)
add_subdirectory(user-space)

set(KERNEL_FRAGMENTS_ASM_SOURCES
    ${KERNEL_FRAGMENTS_ASM_SOURCES}
    PARENT_SCOPE
)

set(KERNEL_FRAGMENTS_C_SOURCES
    ${KERNEL_FRAGMENTS_C_SOURCES}
    PARENT_SCOPE
)

set(ALL_BOOT_SOURCES
    ${ALL_BOOT_SOURCES}
    PARENT_SCOPE)
