#include "../../utils/memory.h"
#include "../../utils/string.h"
#include "../../utils/standard.h"
#include "../pit.h"
#include "vesa_display.h"
#include "../../utils/io.h"
#include "video_buffer.h"
#include <stdio.h>

void clear_row(char row_num) {
    unsigned int line_offset = get_screen_offset(0, row_num);
    for (int i = 0; i < NUM_COLS * CONFIG_BYTE_PERIOD; i++) {
        set_byte_in_video_buffer(line_offset + i, '\0');
    }
}

static unsigned char NUM_WRITEABLE_ROWS;
static unsigned char NUM_WRITEABLE_COLS;

static bool SCROLLING_IS_ENABLED;

// Advances the text cursor, scrolling the video buffer if necessary
int handle_scrolling(int cursor_offset) {

    if (SCROLLING_IS_ENABLED) {
        // continue
    } else {
        return cursor_offset;
    }

    // If the cursor is within the screen, return it unmodified
    if (cursor_offset < NUM_WRITEABLE_ROWS * NUM_WRITEABLE_COLS * CONFIG_BYTE_PERIOD) {
        return cursor_offset;
    }

    // Shuffle the rows back one
    for (unsigned int r = 1; r < NUM_WRITEABLE_ROWS; r++) {
            unsigned int low_row_offset = get_screen_offset(0, r);
            unsigned int high_row_offset = get_screen_offset(0, r - 1);

        for (unsigned int c = 0; c < NUM_WRITEABLE_COLS * CONFIG_BYTE_PERIOD; c++) {
            unsigned int low_row_col_offset = low_row_offset + c;
            unsigned int high_row_col_offset = high_row_offset + c;

            unsigned char new_high_row_col_val = get_byte_in_video_buffer(low_row_col_offset);

            set_byte_in_video_buffer(high_row_col_offset, new_high_row_col_val);
        }
    }

    // Clear the last line
    char last_writeable_row = NUM_WRITEABLE_ROWS - 1;
    clear_row(last_writeable_row);

    // Move the offset back one row, such that it is now on the last
    // row, rather than off the edge of the screen.
    cursor_offset -= CONFIG_BYTE_PERIOD * NUM_WRITEABLE_COLS;

    // Return the updated cursor position
    return cursor_offset;
}

static bool CLOCK_IS_VISIBLE;
static short TIMER_ROW;

void initialize_clock() {
    CLOCK_IS_VISIBLE = false;
    TIMER_ROW = NUM_ROWS - 1;
}

void hide_clock() {
    CLOCK_IS_VISIBLE = false;
    NUM_WRITEABLE_ROWS = NUM_ROWS;

    for (int r = TIMER_ROW; r < NUM_ROWS; r++) {
        clear_row(r);
    }
}

void show_clock() {
    CLOCK_IS_VISIBLE = true;
    NUM_WRITEABLE_ROWS = TIMER_ROW - 1;

    update_clock();
}

void print_int_bottom_left(unsigned int num) {
    int og_cursor_loc = get_cursor_offset();

    int bottom_left_offset = \
        get_screen_offset(0, NUM_ROWS - 1);

    set_cursor(bottom_left_offset);

    char* num_str = int_to_string(num, 10);
    print(num_str);

    set_cursor(og_cursor_loc);
}

void print_clock() {
    bool og_scrolling_is_enabled = SCROLLING_IS_ENABLED;

    SCROLLING_IS_ENABLED = false;

    int tick_count = get_tick_count();
    print_int_bottom_left(tick_count);

    SCROLLING_IS_ENABLED = og_scrolling_is_enabled;
}

void update_clock() {
    if (CLOCK_IS_VISIBLE) {
        print_clock();
    }
}

// Attribute byte for our default color scheme.
#define WHITE_ON_BLACK 0x0f

void print_char(char character, int col, int row, char attribute_byte) {

    /* If attribute byte is zero, assume the default style. */
    if (!attribute_byte) {
        attribute_byte = WHITE_ON_BLACK;
    }

    /* Get the video memory offset for the screen location */
    int offset;

    /* If col and row are non-negative, use them for offset. */
    if (col >= 0 && row >= 0) {
        offset = get_screen_offset(col, row);

    /* Otherwise, use the current cursor position. */
    } else {
        offset = get_cursor_offset();
    }

    // If we see a newline character, set offset to the end of
    // current row, so it will be advanced to the first col
    // of the next row.
    if (character == '\n') {
        unsigned int rows = offset / (CONFIG_BYTE_PERIOD * NUM_COLS);
        offset = get_screen_offset(NUM_COLS - 1, rows);

    // Otherwise, write the character and its attribute byte to
    // video memory at our calculated offset.
    } else {
        set_byte_in_video_buffer(offset, character);
        set_byte_in_video_buffer(offset + 1, attribute_byte);
    }

    // Update the offset to the next character cell, which is
    // two bytes ahead of the current cell.
    offset += CONFIG_BYTE_PERIOD;

    offset = handle_scrolling(offset);

    // Update the cursor position on the screen device.
    set_cursor(offset);
}

void print_at(char* message, int col, int row) {

    // Update the cursor if col and row not negative.
    if (col >= 0 && row >= 0) {
        set_cursor(get_screen_offset(col, row));
    }

    // Loop through each char of the message and print it.
    int i = 0;
    while (message[i] != 0) {
        print_char(message[i], col, row, WHITE_ON_BLACK);
        i++;
    }
}

void print(char * message) {
    print_at(message, -1, -1);
}

void clear_screen() {
    int row = 0;
    int col = 0;

    /* Loop through video memory and write blank characters . */
    for (row = 0; row < NUM_ROWS; row++) {
        for (col = 0; col < NUM_COLS; col++) {
            print_char(' ', col, row, WHITE_ON_BLACK);
        }
    }

    // Move the cursor back to the top left.
    set_cursor(get_screen_offset(0 , 0));
}

void print_int(unsigned int val) {
    char* string_repr = int_to_string(val, 10);
    print(string_repr);
}

void print_int_ln(unsigned int val) {
    char* string_repr = int_to_string(val, 10);
    print_ln(string_repr);
}

void print_int_ln_hex(unsigned int val) {
    char* int_str = int_to_string(val, 16);
    char hex_prefix[] = "0x";

    print(hex_prefix);
    print_ln(int_str);
}

void print_ln(char* message) {
    print(message);

    char newline[] = "\n";
    print(newline);
}

void initialize_screen() {
    clear_screen();

    SCROLLING_IS_ENABLED = true;

    NUM_WRITEABLE_ROWS = NUM_ROWS;
    NUM_WRITEABLE_COLS = NUM_COLS;

    initialize_clock();
    show_clock();
}
