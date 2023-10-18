set(SOURCE_FILES
    driver_isr.asm
    check_queue.asm
    host_to_nic.asm
    initialize_driver.asm
    nic_to_host.asm
    packets.c
    registers.asm
    transmit_next_packet_in_queue.asm
    notifications.c
    functions.c)

include(GetAbsolutePaths)
get_absolute_paths("${SOURCE_FILES}" ABSOLUTE_PATHS)

set(KERNEL_FRAGMENTS_SOURCE_FILES
    ${KERNEL_FRAGMENTS_SOURCE_FILES}
    ${ABSOLUTE_PATHS}
    PARENT_SCOPE
)
