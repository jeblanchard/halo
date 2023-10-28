#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>

#include "kernel/interrupts/idt.c"

static void idt_is_correct_size(void **state) {
    (void) state;

    assert_true(sizeof(idt) == 256 * 8);
}

static void idt_desc_is_correct_size(void **state) {
    (void) state;

    assert_true(sizeof(idt_descriptor) == 8);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(idt_is_correct_size),
        cmocka_unit_test(idt_desc_is_correct_size),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
