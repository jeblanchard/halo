#include "../drivers/vesa-display/vesa_display.c"

void print_welcome_msg() {
    char reached_kernel_msg[] = "Welcome to Halo.\n\n";
    print(reached_kernel_msg);
}

void print_prompt() {
    char reached_kernel_msg[] = ">>> ";
    print(reached_kernel_msg);
}

void start_cmd_line() {
    print_welcome_msg();
    print_prompt();
}