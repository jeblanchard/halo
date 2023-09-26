add_subdirectory(boot)
add_subdirectory(kernel)

# We have to do this in order to propagate
# the value of the variable upwards
set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    PARENT_SCOPE)

set(BOOTSECTOR_AUXILLARY_FILES
    ${BOOTSECTOR_AUXILLARY_FILES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE)
