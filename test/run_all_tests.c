void main() {
    void* all_tests_to_run[] = {
        test_get_unspecified_address,

    } 
}

void run_all_tests(void* pointer_to_test) {
    

    // for each test, check the result
    // show the result for each
}

// what if after every test exit is called so there's a
// address space each time? will that make the tests slower?
// we could use a shell script that calls each test listed.
// however we also don't want all of the testing logic spread out
// over multiple directiores. that coupling without making it easy
// for the user to understand the relationship of.