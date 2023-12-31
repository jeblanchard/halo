#include "drivers/vesa-display/vesa_display.h"
#include "drivers/pit.h"
#include "drivers/8259a-pic/pic.h"
#include "interrupts/idt.h"
#include "gdt.h"
#include "drivers/keyboard.h"
#include "user-space/cmd-line/cmd_line.h"
#include "kernel/memory/phys_mem_mgr.h"
#include "memory/manager.h"
#include "drivers/pci.h"


void initialize_kernel(struct boot_info* boot_info) {
    initialize_screen();

    init_gdt();
    init_idt();

    initialize_pic();

    initialize_pit();
    initialize_keyboard();

    initialize_pci();

    enable_hw_ints();

    print_int_ln((unsigned int) boot_info);
}

/* At this point, we have just entered PM. Hardware
 * interrupts have been disabled in order to allow us to
 * initialize our PM environment. */
void start(struct boot_info* boot_info) {
    initialize_kernel(boot_info);

    for (;;) {}

    start_cmd_line();
}
