cmake_minimum_required(VERSION 3.27.5)

# declare the project name and the
# languages used
project(Halo C ASM_NASM)

# Mark where to find this project's
# modules
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# set(CMAKE_HOST_SYSTEM_NAME Linux)

# Mark where to find cmocka CMake modules
# list(APPEND CMAKE_MODULE_PATH
#     ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmocka/Modules)

# Ensure that cmocka is available to use
# find_package(cmocka)

# Mark where CMake can find the toolchain
# file to use when cross-compiling to Bochs
# set(BOCHS_TOOLCHAIN_FILE_PATH
#     ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Toolchains/bochs.cmake)

# Set CMocka's unit testing flag to on.
# We will compile CMocka unit tests
# every time we build.
# set(UNIT_TESTING ON)

# When we process all of our source
# directories these variables will be
# populated.
set(KERNEL_FRAGMENTS_SOURCE_FILES "")
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
set(KERNEL_ELF kernel.elf)
set(BOOTSECTOR_BIN boot_sector.bin)

add_library(${KERNEL_ENTRY_LIB}
    ${KERNEL_ENTRY_SOURCE_FILE}
)
target_compile_options(${KERNEL_ENTRY_LIB}
    PUBLIC $<$<COMPILE_LANGUAGE:ASM_NASM>:-f elf>
)

add_library(${KERNEL_FRAGMENTS_LIB}
    ${KERNEL_FRAGMENTS_SOURCE_FILES})
target_compile_options(${KERNEL_FRAGMENTS_LIB}
    # PUBLIC $<$<COMPILE_LANGUAGE:ASM_NASM>:-f elf>
    PUBLIC $<$<COMPILE_LANGUAGE:C>:-ffreestanding -Werror -Wextra -Wall -nostdlib -lgcc>)

set(KERNEL_FRAGMENTS_LIB_GOOD_FORMAT ${KERNEL_FRAGMENTS_LIB}_good_format)
add_custom_target(${KERNEL_FRAGMENTS_LIB_GOOD_FORMAT} ALL
    DEPENDS ${KERNEL_FRAGMENTS_LIB}
    COMMAND objcopy -I pe-i386 -O elf32-i386 lib${KERNEL_FRAGMENTS_LIB}.a
        lib${KERNEL_FRAGMENTS_LIB_GOOD_FORMAT}.a
    COMMENT "Building kernel ELF"
)



# target_link_options(${KERNEL_FRAGMENTS_LIB}
#     PUBLIC --oformat elf32-i386
# )

# add_custom_target(${KERNEL_FRAGMENTS_LIB} ALL
#     DEPENDS ${KERNEL_FRAGMENTS_SOURCE_FILES}
#     COMMAND ld -T NUL -o ${KERNEL_ELF} -Ttext 0x1000
#         -l${KERNEL_ENTRY_LIB} -l${KERNEL_FRAGMENTS_LIB}
#         -L. --oformat elf32-i386
#     COMMENT "Building kernel ELF"
# )

include(GetAbsolutePaths)

# We then link these two together,
# and we ensure that the kernel
# entry appears at the beginning
# of the resulting file

add_custom_target(${KERNEL_ELF} ALL
    DEPENDS ${KERNEL_ENTRY_LIB} ${KERNEL_FRAGMENTS_LIB}
    COMMAND ld -T NUL -o ${KERNEL_ELF} -Ttext 0x1000
        -l${KERNEL_ENTRY_LIB} -l${KERNEL_FRAGMENTS_LIB}
        -L.
    COMMENT "Building kernel ELF"
)

add_dependencies(${KERNEL_ELF}
    ${KERNEL_ENTRY_LIB}
    ${KERNEL_FRAGMENTS_LIB}
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
    DEPENDS ${KERNEL_ELF}
    COMMAND objcopy -O binary -j .text ${KERNEL_ELF} ${KERNEL_BIN}
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
