#include "../pit.h"
#include "../../../test/functionality.h"

void get_tick_count_test() {
    int result = get_tick_count();

    char[] test_name = "Get Tick Count";
    if (result == 0) {
        show_test_passed(test_name);
    } else {
        show_test_failed(test_name);
    }
}
