set(SOURCE_FILES
    basic_gdt.asm
    boot_sector.asm
    disk_load.asm
    memory_size.asm
    multiboot2.asm)

include(GetAbsolutePaths)
get_absolute_paths("${SOURCE_FILES}" ABSOLUTE_PATHS)

set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE
)
