set(SOURCE_FILES
    print_hex_rm.asm
    print_string_pm.asm
    print_string_rm.asm  )

include(GetAbsolutePaths)
get_absolute_paths("${SOURCE_FILES}" ABSOLUTE_PATHS)

set(ALL_BOOT_SOURCES
    ${ALL_BOOT_SOURCES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE
)
