set(SOURCE_FILES
    print_hex_rm.asm
    print_string_pm.asm
    print_string_rm.asm  )

include(GetAbsolutePaths)
get_absolute_paths("${SOURCE_FILES}" ABSOLUTE_PATHS)

set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE
)
