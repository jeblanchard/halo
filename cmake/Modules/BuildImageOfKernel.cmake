# Tell CMake to use the Bochs toolchain file
set(CMAKE_TOOLCHAIN_FILE ${BOCHS_TOOLCHAIN_FILE_PATH})


# how do i get CMake to compile all of these files?



add_custom_command(OUTPUT ${KERNEL_ENTRY_OBJ_FILE}

    # depends on the kernel entry and kernel
    # object files
    DEPENDS ${KERNEL_ENTRY_SRC_FILE}

    COMMAND nasm -f elf ${KERNEL_ENTRY_SRC_FILE} -o ${KERNEL_ENTRY_OBJ_FILE}

    COMMENT "Building Halo OS image.")

# link all of the kernel fragments into one object
# file
add_custom_command(OUTPUT ${KERNEL_OBJ_FILE}

    # depends on the kernel entry and kernel
    # object files
    DEPENDS ${KERNEL_ENTRY_OBJ_FILE} ${KERNEL_FRAGMENTS_OBJ_FILES}

    COMMAND ld -T NUL -o ${KERNEL_OBJ_FILE} -Ttext 0x1000 \ 
        ${KERNEL_ENTRY_OBJ_FILE} ${KERNEL_FRAGMENTS_OBJ_FILES}
    COMMENT "Building Halo OS image.")

# link all of the kernel fragments into one object
# file
add_custom_command(OUTPUT ${KERNEL_OBJ_FILE}

    # depends on the kernel entry and kernel
    # object files
    DEPENDS ${KERNEL_ENTRY_OBJ_FILE} ${KERNEL_FRAGMENTS_OBJ_FILES}

    COMMAND ld -T NUL -o ${KERNEL_OBJ_FILE} -Ttext 0x1000 \ 
        ${KERNEL_ENTRY_OBJ_FILE} ${KERNEL_FRAGMENTS_OBJ_FILES}
    COMMENT "Building Halo OS image.")

# Take only the bare-metal usable binary from the kernel's
# object file
add_custom_command(OUTPUT ${KERNEL_BIN_FILE}

    # depends on the kernel entry and kernel
    # object files
    DEPENDS ${KERNEL_OBJ_FILE}

    COMMAND objcopy -O binary -j .text ${KERNEL_OBJ_FILE} ${KERNEL_BIN_FILE}
    COMMENT "Building Halo OS image.")

# Compile the bootsector to binary
add_custom_command(OUTPUT ${BOOTSECTOR_BIN_FILE}

    # depends on the kernel entry and kernel
    # object files
    DEPENDS ${BOOT_SECTOR_SOURCE_FILES}

    COMMAND nasm ${BOOT_SECTOR_SOURCE_FILES} -f bin -o ${BOOTSECTOR_BIN_FILE}
    COMMENT "Building Halo OS image.")


# this is where we finalize the OS image to later
# feed to Bochs
set(HALO_OS_BIN_FILE bochs/halo-os-image.bin)
add_custom_command(OUTPUT ${HALO_OS_BIN_FILE}

    # depends on the kernel entry and kernel
    # object files
    DEPENDS ${BOOTSECTOR_BIN_FILE} ${KERNEL_BIN_FILE}

    # the commands that build the OS
    COMMAND cat ${BOOTSECTOR_BIN_FILE} ${KERNEL_BIN_FILE} > ${HALO_OS_BIN_FILE}
    COMMENT "Building Halo OS image.")

# this will set off the processing of all
# CMake files in the source tree
add_subdirectory(${SOURCE_DIR})

# and some commands that create the actual kernel image

# and finally some commands that concatenate those two
# into a complete kernel image to pass on to Bochs
