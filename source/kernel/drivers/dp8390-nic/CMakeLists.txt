set(C_SOURCES
    packets.c
    notifications.c
    functions.c)

set(ASM_SOURCES
    driver_isr.asm
    check_queue.asm
    host_to_nic.asm
    initialize_driver.asm
    nic_to_host.asm
    registers.asm
    transmit_next_packet_in_queue.asm
)


include(GetAbsolutePaths)

get_absolute_paths("${C_SOURCES}" C_SOURCES_ABS)
get_absolute_paths("${ASM_SOURCES}" ASM_SOURCES_ABS)

set(KERNEL_FRAGMENTS_C_SOURCES
    ${KERNEL_FRAGMENTS_C_SOURCES}
    ${C_SOURCES_ABS}
    PARENT_SCOPE
)

set(KERNEL_FRAGMENTS_ASM_SOURCES
    ${KERNEL_FRAGMENTS_ASM_SOURCES}
    ${ASM_SOURCES_ABS}
    PARENT_SCOPE
)
