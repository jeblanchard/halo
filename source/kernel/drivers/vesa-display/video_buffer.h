void set_byte_in_video_buffer(unsigned int offset, unsigned char val);

unsigned char get_byte_in_video_buffer(unsigned int offset);

unsigned int get_cursor_offset();

void set_cursor(unsigned int cell_offset);

unsigned int get_screen_offset(unsigned int col, unsigned int row);

#define NUM_ROWS 25
#define NUM_COLS 80
#define CONFIG_BYTE_PERIOD 2
