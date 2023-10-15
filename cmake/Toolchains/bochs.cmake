# ensure that CMake can find the platform file
get_filename_component(_thisDir
    "${CMAKE_CURRENT_LIST_FILE}" PATH)
set(CMAKE_MODULE_PATH "${_thisDir}/Modules" ${CMAKE_MODULE_PATH})

set(CMAKE_SYSTEM_NAME Generic)

set(CMAKE_C_COMPILER C:/MinGW/bin/gcc.exe)

set(CMAKE_ASM_COMPILER C:/"Program Files"/NASM/nasm.exe)

# set(CMAKE_SYSTEM_PROCESSOR i386)
