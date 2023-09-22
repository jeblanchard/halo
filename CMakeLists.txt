# declare the project name and the
# languages used
project(Halo C ASM_NASM)

# create the test source variable with
# a top level scope
set(TEST_SRC_FILES "")

# this will set off the processing of all
# CMake files in the source tree
add_subdirectory(source)

set(CMAKE_MODULE_PATH cmake/Modules)

# Add path to cmocka CMake modules. We'll
# need this to create the executable for our
# tests.
list(APPEND CMAKE_MODULE_PATH
     /c/"Program Files"/cmocka-1.1.7/cmake/Modules)

# At this point, the names of all test
# source files should have been collected.
# Here we create one executable that
# contains all of the project's tests.

# (I think I'll have to change this setup)
add_executable(all_tests
    ${TEST_SRC_FILES})

# If we are testing, build all of the test
# executables. If we aren't, build the
# OS image.

# How do we best optionally set up the
# executable that holds our tests?
if (UNIT_TESTING)
    find_package(cmocka)
    include(AddCMockaTest)
    add_subdirectory(test)
endif()

# We need to use an executable that controls
# the eventual os-image for Bochs.
set(CMAKE_TOOLCHAIN_FILE cmake/TC-bochs.cmake)

# write a set of commands that create the kernel entry image

# and some commands that create the actual kernel image

# and finally some commands that concatenate those two
# into a complete kernel image to pass on to Bochs

# this is where we finalize the OS image to later
# feed to Bochs
add_custom_command(OUTPUT bochs/halo-os-image.bin
    COMMAND # the commands that build the OS
    DEPENDS # the kernel entry and kernel object files
    COMMENT "Building Halo OS image."

    )

