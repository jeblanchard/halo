#include "drivers/screen.h"
#include "drivers/pit.h"
#include "drivers/pic.h"
#include "idt.h"
#include "gdt.h"

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

    initialize_idt();
    initialize_pic();

//    enable_hardware_interrupts();

    initialize_pit();

    char init_msg[] = "Finished initialization.\n";
    print(init_msg);

    char message[] = "Welcome to Halo.\nThe greatest OS ever created.";
    print(message);

//    unsigned char pit_int_num = 10;
//    gen_interrupt(pit_int_num);

    // Infinite loop.
    for (;;) {};
}
