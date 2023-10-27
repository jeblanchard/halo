#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdbool.h>
#include <stdlib.h>
#include "kernel/utils/memory.h"
#include <stdio.h>

#include "../message.c"

static int test_queue_msg_for_tx_teardown(void **state) {
    (void) state;

    set_memory(&tx_queue_buffer, 0, TX_RX_BUFFER_SIZE);

    return 0;
}

static void test_queue_msg_for_tx(void **state) {
    (void) state;

    unsigned char msg[5] = {1, 2, 3, 4, 5};

    queue_msg_for_tx_result result = queue_msg_for_tx(msg, 5);
    assert_true(result == SUCCESSFULLY_QUEUED_MSG);

    assert_true(tx_queue_buffer[0] == 1);
    assert_true(tx_queue_buffer[1] == 2);
    assert_true(tx_queue_buffer[2] == 3);
    assert_true(tx_queue_buffer[3] == 4);
    assert_true(tx_queue_buffer[4] == 5);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_teardown(test_queue_msg_for_tx, test_queue_msg_for_tx_teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
