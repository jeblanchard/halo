#include "drivers/screen.h"
#include "drivers/pit.h"
#include "drivers/8259a-pic/pic.h"
#include "interrupts/idt.h"
#include "gdt.h"
#include "drivers/keyboard.h"
#include "cmd-line/main.h"
#include "boot.h"
#include "physical-memory-management/manager.h"
#include "drivers/pci/functions.h"


void initialize_kernel(struct multiboot2_info* boot_info) {
    initialize_screen();

    initialize_gdt();
    initialize_idt();

    initialize_pic();

    initialize_pit();
    initialize_keyboard();

    initialize_pci();

    enable_hardware_interrupts();

    print_int_ln((unsigned int) boot_info);
}

/* At this point, we have just entered PM. Hardware
 * interrupts have been disabled in order to allow us to
 * initialize our PM environment. */
void start(struct multiboot2_info* boot_info) {
    initialize_kernel(boot_info);

    for (;;) {}

    start_cmd_line();
}
