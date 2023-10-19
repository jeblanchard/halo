set(C_SOURCES
    interrupt_handler.c
    uart.c)

set(ASM_SOURCES
    registers.asm
    initialization.asm
)

include(GetAbsolutePaths)

get_absolute_paths("${C_SOURCES}" C_SOURCES_ABS)
get_absolute_paths("${ASM_SOURCES}" ASM_SOURCES_ABS)

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
