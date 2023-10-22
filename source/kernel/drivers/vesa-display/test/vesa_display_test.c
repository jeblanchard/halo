#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdbool.h>
#include "../vesa_display.h"
#include "../video_buffer.h"

#define FAKE_VIDEO_BUFFER_SIZE 20

static unsigned char fake_video_buffer[FAKE_VIDEO_BUFFER_SIZE];

void __wrap_set_byte_in_video_buffer(unsigned int offset, unsigned char val) {
    fake_video_buffer[offset] = val;
}

unsigned char __wrap_get_byte_in_video_buffer(unsigned int offset) {
    return fake_video_buffer[offset];
}

static int test_print_teardown(void **state) {
    (void) state;

    for (unsigned char i = 0; i < FAKE_VIDEO_BUFFER_SIZE; i++) {
        fake_video_buffer[i] = 0;
    }

    return 0;
}

unsigned char __wrap_port_byte_in(unsigned short port) {
    (void) port;

    return 0;
}

void __wrap_port_byte_out(unsigned short port, unsigned char data) {
    (void) port;
    (void) data;
}

static unsigned int cursor;

unsigned int __wrap_get_cursor_offset() {
    unsigned int old_cursor = cursor;
    cursor += CONFIG_BYTE_PERIOD;

    return old_cursor;
}

void __wrap_set_cursor() {}

unsigned int __wrap_get_screen_offset(int col, int row) {
    (void) col;
    (void) row;
    return 0;
}

#define MSG_SIZE 8

static void test_print(void **state) {
    (void) state;

    char msg[MSG_SIZE] = {'W', 'e', 'l', 'c', 'o', 'm', 'e', '\0'};
    print(msg);

    assert_int_equal(msg[0], fake_video_buffer[0]);
    assert_int_equal(msg[1], fake_video_buffer[1 * CONFIG_BYTE_PERIOD]);
    assert_int_equal(msg[2], fake_video_buffer[2 * CONFIG_BYTE_PERIOD]);
    assert_int_equal(msg[3], fake_video_buffer[3 * CONFIG_BYTE_PERIOD]);
    assert_int_equal(msg[4], fake_video_buffer[4 * CONFIG_BYTE_PERIOD]);
    assert_int_equal(msg[5], fake_video_buffer[5 * CONFIG_BYTE_PERIOD]);
    assert_int_equal(msg[6], fake_video_buffer[6 * CONFIG_BYTE_PERIOD]);
    assert_int_equal(msg[7], fake_video_buffer[7 * CONFIG_BYTE_PERIOD]);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_teardown(test_print, test_print_teardown),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
