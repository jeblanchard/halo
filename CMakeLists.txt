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

# Ensure that cmocka is available to use
find_package(cmocka)

# Mark where CMake can find the toolchain
# file to use when cross-compiling to Bochs
# set(BOCHS_TOOLCHAIN_FILE_PATH
#     ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Toolchains/bochs.cmake)

# Set CMocka's unit testing flag to on.
# We will compile CMocka unit tests
# every time we build.
set(UNIT_TESTING ON)

# When we process all of our source
# directories these variables will be
# populated.
set(KERNEL_FRAGMENTS_SOURCE_FILES "")

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

# Set our ELF names
set(KERNEL_ENTRY_ELF multiboot2.elf)
set(KERNEL_FRAGMENTS_ELF kernel_fragments.elf)
set(KERNEL_ELF kernel.elf)
set(BOOTSECTOR_BIN boot_sector.bin)

# Now we add ELFs for our kernel
# entry point, all of the kernel fragments,
# and the bootsector

# add_executable(${KERNEL_ENTRY_ELF} 
#     ${KERNEL_ENTRY_SOURCE_FILE})
# target_compile_options(${KERNEL_ENTRY_ELF}
#     PUBLIC $<$<COMPILE_LANGUAGE:ASM_NASM>:-f elf>)

add_custom_command(
    OUTPUT ${KERNEL_ENTRY_ELF}
    COMMAND nasm -f elf ${KERNEL_ENTRY_SOURCE_FILE}
    DEPENDS ${KERNEL_ENTRY_SOURCE_FILE}
)

add_executable(${KERNEL_FRAGMENTS_ELF}
    ${KERNEL_FRAGMENTS_SOURCE_FILES})
target_compile_options(${KERNEL_FRAGMENTS_ELF}
    PUBLIC $<$<COMPILE_LANGUAGE:ASM_NASM>:-f elf>
    PUBLIC $<$<COMPILE_LANGUAGE:C>:-ffreestanding -o -Werror -Wextra -Wall -nostdlib -lgcc>)

# We then link these two together using
# a custom command to ensure that the
# kernel entry appears at the beginning
# of the resulting file
add_custom_command(
    OUTPUT ${KERNEL_ELF}
    COMMAND ld -T NUL -o ${KERNEL_ELF} -Ttext \ 
        0x1000 ${KERNEL_ENTRY_ELF} ${KERNEL_FRAGMENTS_ELF}
    DEPENDS ${KERNEL_ENTRY_ELF} ${KERNEL_FRAGMENTS_ELF})

# For the bootsector, we can compile
# directly to binary format
set(BOOTSECTOR_SOURCE_FILE source/boot/boot_sector.asm)


# add_executable(${BOOTSECTOR_BIN}
#     ${BOOTSECTOR_SOURCE_FILE})
# target_compile_options(${BOOTSECTOR_BIN}
#     PUBLIC -f bin -o ${BOOTSECTOR_BIN})
add_custom_command(
    OUTPUT ${BOOTSECTOR_BIN}
    COMMAND nasm ${BOOTSECTOR_SOURCE_FILE} -f bin -o ${BOOTSECTOR_BIN}

    # the boot sector auxillary files were populated when we
    # added the boot directory
    DEPENDS ${BOOTSECTOR_SOURCE_FILE} ${BOOTSECTOR_AUXILLARY_FILES}
)

# Now we convert the kernel's ELF file
# to binary that hardware can run
set(KERNEL_BIN kernel.bin)
add_custom_command(
    OUTPUT ${KERNEL_BIN}
    DEPENDS ${KERNEL_ELF}
    COMMAND objcopy -O binary ${KERNEL_ELF} ${KERNEL_BIN}
)

# To finish we combine the binaries of
# our bootsector and kernel into a
# singular image
set(HALOS_BIN halos.bin)
add_custom_command(
    OUTPUT ${HALOS_BIN}
    DEPENDS ${BOOTSECTOR_BIN} ${KERNEL_BIN}
    COMMAND cat ${BOOTSECTOR_BIN} ${KERNEL_BIN} > ${HALOS_BIN}
)
