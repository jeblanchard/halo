cmake_minimum_required(VERSION 3.10)

project(Halo VERSION 1.0)

# specify the C standard
add_library(halo_compiler_flags INTERFACE)
target_compile_features(halo_compiler_flags INTERFACE c_std_99)

set(gcc_like_c "$<COMPILE_LANG_AND_ID:C,ARMClang,AppleClang,Clang,GNU>")
set(msvc_c "$<COMPILE_LANG_AND_ID:C,MSVC>")
target_compile_options(halo_compiler_flags INTERFACE
    "$<${gcc_like_c}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
    "$<${msvc_c}:$<BUILD_INTERFACE:-W3>>"
)

# Control where the static and shared libraries are
# built so that on Windows we don't need to tinker
# with the PATH to run the executable
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

# Configure a header file to pass the version number only
configure_file(HaloConfig.h.in HaloConfig.h)

# add the cmocka libary
add_subdirectory(cmocka)

# add the executable
add_executable(Halo )
target_link_libraries(Halo PUBLIC cmocka)

if(USE_CMOCKA)
    add_subdirectory(cmocka)
    list(APPEND EXTRA_LIBS cmocka)
endif()

target_include_directories(Halo PUBLIC
                           "${PROJECT_BINARY_DIR}"
                          )

target_include_directories(cmocka
            INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
            )

# Enable dashboard scripting
include(CTest)

# Does the application run?
add_test(NAME Runs COMMAND Halo)

# Does the usage message work?
add_test(NAME Usage COMMAND Halo)
set_tests_properties(Halo
    PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*number"
    )

# Define a function to simplify adding tests
function(do_test target arg result)
    add_test(NAME Comp${arg} COMMAND ${target} ${arg})
    set_tests_properties(Comp${arg})
        PROPERTIES PASS_REGULAR_EXPRESSION ${result}
        )
endfunction(do_test)

# Do a bunch of result based tests
do_test(Tutorial 4 "4 is 2")
do_test(Tutorial 9 "9 is 3")
do_test(Tutorial 5 "5 is 2.236")
do_test(Tutorial 7 "7 is 2.645")
do_test(Tutorial 25 "25 is 5")
do_test(Tutorial -25 "-25 is [-nan|nan|0]")
do_test(Tutorial 0.0001 "0.0001 is 0.01")

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${Tutorial_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Tutorial_VERSION_MINOR}")
include(CPack)

install(EXPORT cmockaTargets
    FILE cmocka
    DESTINATION lib/cmake/cmocka


include(CMakePackageConfigHelpers)

# Generate the config file that includes the exports
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake
    "${CMAKE_CURRENT_BINARY_DIR}/cmocka.cmake"
    INSTALL_DESTINATION "lib/cmake/example"
    NO_SET_AND_CHECK_MACRO
    NO_CHECK_REQUIRED_COMPONENTS_MACRO
    )

# Generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/cmocka.cmake"
    VERSION "${Halo_VERSION_MAJOR}.${Halo_VERSION_MINOR}"
    COMPATIBILITY AnyNewerVersion
)

# install the configuration file
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/cmocka.cmake
    DESTINATION lib/cmake/cmocka
    )

# Add CMocka CMake modules
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmocka)

if (UNIT_TESTING)
    find_package(cmocka 1.1.0 REQUIRED)
    include(AddCMockaTest)
    add_subdirectory(test)
endif()


