# collect the boot files
include(boot/CMakeLists.txt)
foreach(FILE ${FILES})
    set(bootFiles ${bootFiles} boot/${FILE})
endforeach()

# collect the kernel files
include(kernel/CMakeLists.txt)
foreach(FILE ${FILES})
    set(kernelFiles ${kernelFiles} kernel/${FILE})
endforeach()

# add the source files to the executable
add_executable(halo ${bootFiles} ${kernelFiles})
