#include "drivers/screen.h"
#include "drivers/pit.h"
#include "drivers/pic.h"
#include "idt.h"
#include "gdt.h"
#include "drivers/keyboard.h"
#include "cmd-line/main.h"
#include "boot.h"

void initialize_kernel() {
    initialize_screen();

    initialize_gdt();
    initialize_idt();
    initialize_pic();

    initialize_pit();
    initialize_keyboard();

    enable_hardware_interrupts();
}

/* At this point, we have just entered PM. Hardware
 * interrupts have been disabled in order to allow us to
 * initialize our PM environment. */
void start(struct multiboot2_info* boot_info) {
    initialize_kernel();

    char boot_info_msg[] = "Total size of boot info: ";
    print(boot_info_msg);
    print_int_ln(boot_info -> total_size);

    for (;;) {}

    start_cmd_line();
}
