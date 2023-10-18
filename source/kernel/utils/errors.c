#include "../drivers/vesa_display.h"

void halt_and_display_error_msg(char * message) {
    print_ln(message);

    for (;;) {}
}
