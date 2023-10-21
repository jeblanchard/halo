cmake_minimum_required(VERSION 3.27.5)

# declare the project name and the
# languages used
project(Halo C ASM_NASM)

# Mark where to find this project's
# modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# Mark where to find cmocka CMake modules
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmocka/Modules)

if (UNIT_TESTING)

    # Ensure that cmocka is available to use
    find_package(CMocka)
    include(AddCMockaTest)
    # add_subdirectory(test)
endif()

# When we process all of our source
# directories these variables will be
# populated.
set(KERNEL_FRAGMENTS_C_SOURCES "")
set(KERNEL_FRAGMENTS_ASM_SOURCES "")
set(ALL_BOOT_SOURCES "")

# We add this here so that any NASM include
# statements can find the files
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Process all of our source code.
# This call also serves the function
# of building all of our tests.
add_subdirectory(source)

set(KERNEL_ENTRY_SOURCE_FILE source/boot/multiboot2.asm)

# We need to separate the kernel's entry
# point from the rest of the kernel
# fragments
list(REMOVE_ITEM
    KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_ENTRY_SOURCE_FILE})

set(KERNEL_ENTRY_LIB kernel_entry)
set(KERNEL_FRAGMENTS_LIB kernel_fragments)
set(KERNEL_ELF kernel)
set(BOOTSECTOR_BIN boot_sector.bin)

add_library(${KERNEL_ENTRY_LIB}
    ${KERNEL_ENTRY_SOURCE_FILE}
)
target_compile_options(${KERNEL_ENTRY_LIB}
    PUBLIC $<$<COMPILE_LANGUAGE:ASM_NASM>:-f elf>
)

set(KERNEL_FRAGMENTS_ASM_LIB kernel_fragments_asm_sources)

add_library(${KERNEL_FRAGMENTS_ASM_LIB}
    ${KERNEL_FRAGMENTS_ASM_SOURCES}
)

target_compile_options(${KERNEL_FRAGMENTS_ASM_LIB}
    PUBLIC $<$<COMPILE_LANGUAGE:ASM>:-f elf>
)

set(KERNEL_FRAGMENTS_C_LIB kernel_fragments_c_sources)

add_library(${KERNEL_FRAGMENTS_C_LIB}
    ${KERNEL_FRAGMENTS_C_SOURCES})

target_compile_options(${KERNEL_FRAGMENTS_C_LIB}
    PUBLIC $<$<COMPILE_LANGUAGE:C>:-c -ffreestanding -Werror -Wextra -Wall -nostdlib -lgcc>)

set(KERNEL_FRAGMENTS_C_LIB_GOOD_FORMAT kernel_fragments_c_lib_good_format.o)

add_custom_target(${KERNEL_FRAGMENTS_C_LIB_GOOD_FORMAT} ALL
    DEPENDS ${KERNEL_FRAGMENTS_C_LIB}
    COMMAND objcopy -O elf32-i386 lib${KERNEL_FRAGMENTS_C_LIB}.a
        ${KERNEL_FRAGMENTS_C_LIB_GOOD_FORMAT}
    COMMENT "Converting object file format of kernel fragment library from C sources"
)

include(GetAbsolutePaths)

# We then link these two together,
# and we ensure that the kernel
# entry appears at the beginning
# of the resulting file

add_custom_target(${KERNEL_ELF} ALL
    DEPENDS ${KERNEL_ENTRY_LIB} ${KERNEL_FRAGMENTS_C_LIB_GOOD_FORMAT}
        ${KERNEL_FRAGMENTS_ASM_LIB}
    COMMAND ld -T NUL -o ${KERNEL_ELF} -shared -Ttext
        0x1000 lib${KERNEL_ENTRY_LIB}.a ${KERNEL_FRAGMENTS_C_LIB_GOOD_FORMAT}
        lib${KERNEL_FRAGMENTS_ASM_LIB}.a
    COMMENT "Building kernel ELF"
)

set(KERNEL_ELF_GOOD_FORMAT kernel_elf_good_format)

add_custom_target(${KERNEL_ELF_GOOD_FORMAT} ALL
    DEPENDS ${KERNEL_ELF}
    COMMAND objcopy -O elf32-i386 ${KERNEL_ELF}
        ${KERNEL_ELF}
    COMMENT "Converting format of kernel object file to ELF"
)

set(BOOTSECTOR_SOURCE_REL source/boot/boot_sector.asm)

# We need to separate the boot sector
# from the rest of the boot sources

list(REMOVE_ITEM
    ALL_BOOT_SOURCES
    ${BOOTSECTOR_SOURCE_REL})

# For the bootsector, we can compile
# directly to binary format

get_absolute_paths(${BOOTSECTOR_SOURCE_REL} BOOTSECTOR_SOURCE_ABS)
get_absolute_paths("${ALL_BOOT_SOURCES}" BOOTSECTOR_DEPS_ABS)

add_custom_target(${BOOTSECTOR_BIN} ALL

    # the boot sector auxillary files were populated when we
    # added the boot directory
    DEPENDS ${BOOTSECTOR_SOURCE_ABS} ${BOOTSECTOR_DEPS_ABS}
    COMMAND nasm ${BOOTSECTOR_SOURCE_ABS} -f bin -o ${BOOTSECTOR_BIN}
        -i${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Building bootsector binary"
)

# Now we convert the kernel's ELF file
# to binary that hardware can run

set(KERNEL_BIN kernel.bin)

add_custom_target(${KERNEL_BIN} ALL
    DEPENDS ${KERNEL_ELF_GOOD_FORMAT}
    COMMAND objcopy -O binary ${KERNEL_ELF} ${KERNEL_BIN}
    COMMENT "Building kernel binary"
)

# To finish we combine the binaries of
# our bootsector and kernel into a
# single image

set(HALOS_BIN halos.bin)

add_custom_target(${HALOS_BIN} ALL
    DEPENDS ${HALOS_BIN}
    COMMAND cat ${BOOTSECTOR_BIN} ${KERNEL_BIN} > ${HALOS_BIN}
    COMMENT "Building final OS image."
)
