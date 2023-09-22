# ensure that CMake can find the platform file
get_filename_component(_thisDir
    "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(CMAKE_MODULE_PATH "${_thisDir}/Modules" ${CMAKE_MODULE_PATH})

# specify that there is no OS present
set(CMAKE_SYSTEM_NAME Generic)

# configure the C compiler
set(CMAKE_C_COMPILER C:/MinGW/bin/gcc.exe ${GCC_OPTIONS})

# configure ASM compiler
set(NASM_OPTIONS "-f elf")
set(CMAKE_ASM_COMPILER C:/"Program Files"/NASM/nasm.exe ${NASM_OPTIONS})

# Specify the processor we are cross-compiling
# for. This is a processor that Bochs can emulate.
set(CMAKE_SYSTEM_PROCESSOR i386)

