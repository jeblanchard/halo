enum {
    PASS = 1,
    FAIL = 0
} test_result;

struct {
    void* function;
    char[] name;
} test;
