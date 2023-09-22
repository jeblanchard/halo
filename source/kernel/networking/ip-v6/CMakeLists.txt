target_include_directories(address_test
    PUBLIC "${CMOCKA_INCLUDE_DIRS}")

# Collect the tests for this directory
include(AddCMockaTest)

add_cmocka_test(address_test
    SOURCES address_test.c address.c
    LINK_LIBRARIES cmocka)

add_cmocka_test(icmp_v6_test
    SOURCES icmp_v6_test.c icmp_v6.c
    LINK_LIBRARIES cmocka)



