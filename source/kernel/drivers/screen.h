// Prints a string at the current
// cursor position.
void print(char* message);

// Same as print() but followed by a
// newline.
void print_ln(char* message);

// Prints a character at the specified cell. If row or col is negative,
// prints the char at the current cursor position. If the
// attribute_byte is 0, the char is printed in the default
// white-on-black style.
void print_char(char character, int col, int row, char attribute_byte);

// Prints a char at a particular row and col
// of the screen.
void print_at(char* message, int col, int row);

// Clears the screen.
void clear_screen();

// Sets the cursor to the provided cell offset.
void set_cursor(int cell_offset);

// Prints the string representation of the
// val provided (in base 10). Followed by
// a newline.
void print_int_ln(int val);

void print_int(int val);

void print_int_ln_hex(int val);

// Prints num at the bottom left of the screen, then
// returns the cursor back to where it was when the
// call was made.
void print_int_bottom_left(int num);

void hide_clock();

void show_clock();

void update_clock();

void initialize_screen();
