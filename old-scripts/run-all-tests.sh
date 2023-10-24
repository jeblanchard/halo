# compile all test code using normal to-host
# gcc compilation. because the test code makes
# calls to the OS code, it should be chillen.

# rest the test file

# the compiler can link the two sides together
# even though they were compiled in a different way.
# is this even possible though?

# i need to pass options to the linker if i'm going to make
# use of mocking

# i need to have an automated way of including a line like:
#     ld -wrap function_name
