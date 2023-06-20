#include "drivers/screen.h"

void start() {

    // We do this to ensure a clean screen as well as a known cursor position
    // for our kernel to work with.
    clear_screen();

    char message[] = "Welcome to Halo.\nThe greatest OS ever created.";
    print(message);
}
