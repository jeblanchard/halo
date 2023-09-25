set(SOURCE_FILES
    call_function_at_address.asm
    errors.c
    io.c
    memory.c
    standard.c
    "string.c")

include(GetAbsolutePaths)
set(ABSOLUTE_PATHS "")
get_absolute_paths("${SOURCE_FILES}" ABSOLUTE_PATHS)

set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE
)
