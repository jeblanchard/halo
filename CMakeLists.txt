cmake_minimum_required(VERSION 3.27.5)

# declare the project name and the
# languages used
project(Halo C ASM_NASM)

# When we process all of our source
# directories these variables will be
# populated.
set(OS_IMAGE_SOURCE_FILES "")
set(TESTING_SOURCE_FILES "")

# process all of our source code
add_subdirectory(source)

# Mark where to find this project's
# modules
set(CMAKE_MODULE_PATH cmake/Modules)

# Mark where to find cmocka CMake modules
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmocka/Modules)

# Ensure that cmocka is available to use
find_package(cmocka)

# Mark where CMake can find the toolchain
# file to use when cross-compiling to Bochs
set(BOCHS_TOOLCHAIN_FILE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Toolchains/bochs.cmake)

# Set CMocka's unit testing flag to on.
# We will compile all CMocka unit tests
# every time we build.
set(UNIT_TESTING ON)

# build our unit tests
# into native executables
include(BuildCMockaUnitTests)

# Build the OS image that will
# later be fed to Bochs
include(BuildImageOfKernel)

# Move our OS image to the Bochs
# program files
include(MoveOSImageToBochs)
