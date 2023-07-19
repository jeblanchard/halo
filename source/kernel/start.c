#include "drivers/screen.h"
#include "drivers/pit.h"
#include "drivers/pic.h"
#include "idt.h"
#include "gdt.h"
#include "drivers/keyboard.h"

// At this point, we have just entered PM. Hardware
// interrupts have been disabled in order to allow us to
// initialize our PM environment.
void start() {

    // We do this to ensure a clean screen as well
    // as a known cursor position for our kernel
    // to work with.
    clear_screen();

    char reached_kernel_msg[] = "Reached kernel.\n";
    print(reached_kernel_msg);

    initialize_gdt();
    initialize_idt();
    initialize_pic();
    initialize_pit();
    initialize_keyboard();

    char init_msg[] = "Finished initialization.\n";
    print(init_msg);

    char message[] = "\nWelcome to Halo.\nThe greatest OS ever created.";
    print(message);

    enable_hardware_interrupts();

    // Infinite loop.
    int i = 0;
    for (; i < 100; i++) {
        gen_interrupt();
//        char loop_msg[] = "Looped.";
//        print(loop_msg);
    };
}
