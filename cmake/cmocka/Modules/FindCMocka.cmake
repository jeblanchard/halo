set (CMOCKA_PATH C:/"Program Files"/cmocka-1.1.7
    CACHE STRING "Windows cmocka path")

# Search for the CMocka include directory
find_path(CMOCKA_INCLUDE_DIR
  NAMES cmocka.h
  PATHS ${CMOCKA_PATH}/include
  DOC "Where the CMocka header file can be found"
)
set(CMOCKA_INCLUDE_DIRS "${CMOCKA_INCLUDE_DIR}")

# Search for the CMocka library directory
find_library(CMOCKA_LIBRARY
  NAMES cmocka
  PATHS ${CMOCKA_PATH}/lib
  DOC "Where the CMocka library can be found"
)
set(CMOCKA_LIBRARIES "${CMOCKA_LIBRARY}")

# Set CMOCKA_FOUND (if all required vars are found)
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cmocka
    DEFAULT_MSG
    CMOCKA_INCLUDE_DIRS
    CMOCKA_LIBRARIES)

# Hide advanced variables from the cmake GUIs.
mark_as_advanced(CMOCKA_PATH
    CMOCKA_INCLUDE_DIR
    CMOCKA_INCLUDE_DIRS
    CMOCKA_LIBRARY
    CMOCKA_LIBRARIES)
