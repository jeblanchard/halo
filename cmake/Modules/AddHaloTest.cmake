# Mark where to find cmocka CMake modules
list(APPEND CMAKE_MODULE_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmocka/Modules)

# Ensure that cmocka is available to use
find_package(CMocka)
include(AddCMockaTest)

macro(add_halo_test)
    set(options "")
    set(oneValueArgs NAME LINK_OPTIONS)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(
        ADD_HALO_TEST
        "${options}"
        "${oneValueArgs}"
        "${multiValueArgs}"
        ${ARGN})

    set(COMPILE_OPTIONS -g -Wall)

    add_cmocka_test(${ADD_HALO_TEST_NAME}
        SOURCES ${ADD_HALO_TEST_SOURCES}
        COMPILE_OPTIONS ${COMPILE_OPTIONS}
        LINK_LIBRARIES ${CMOCKA_LIBRARY}
        LINK_OPTIONS ${ADD_HALO_TEST_LINK_OPTIONS}
    )

    target_include_directories(${ADD_HALO_TEST_NAME}
        PUBLIC ${CMOCKA_INCLUDE_DIR})

endmacro()
