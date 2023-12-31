#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdbool.h>

#include "kernel/memory/manager.c"
#include "kernel/memory/phys_mem_mgr.h"

#define FAKE_BYTES_PER_MEM_BLOCK 4096

alloc_block_resp __wrap_alloc_block() {
    alloc_block_resp* resp = mock_ptr_type(alloc_block_resp*);
    return *resp;
}


#define NUM_BYTES_HALLOCED 20
char fake_physical_mem_block[NUM_BYTES_HALLOCED];

alloc_block_resp alloc_block_success_resp = \
    {status: ALLOC_BLOCK_SUCCESS, buffer_base_addr: (physical_address) &fake_physical_mem_block};

static void alloc_page_test(void **state) {
    (void) state;

    will_return(__wrap_alloc_block, (uintptr_t) &alloc_block_success_resp);

    halloc_resp halloc_resp = halloc(NUM_BYTES_HALLOCED);

    assert_true(halloc_resp.status == HALLOC_SUCCESS);

    char* mem = (char*) halloc_resp.buffer;

    for (int i = 0; i < NUM_BYTES_HALLOCED; i++) {
        mem[i] = 'a';
    }

    for (int i = 0; i < NUM_BYTES_HALLOCED; i++) {
        assert_true(mem[i] == 'a');
    }
}

alloc_block_resp no_free_blocks_resp = {status: ALLOC_BLOCK_ALL_BLOCKS_IN_USE, buffer_base_addr: 0};

static void halloc_not_enough_mem(void **state) {
    (void) state;

    will_return(__wrap_alloc_block, (uintptr_t) &no_free_blocks_resp);

    halloc_resp halloc_resp = halloc(20);

    assert_true(halloc_resp.status == NOT_ENOUGH_MEM);
    assert_true(halloc_resp.buffer == NULL);
}

static void free(void **state) {
    (void) state;

    // allocate one block worth as a setup
    // call the free function
    // assert that that block is free
    // and the number of free blocks was
    // adjusted properly

    assert_true(false);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(alloc_page_test),
        cmocka_unit_test(halloc_not_enough_mem),
        cmocka_unit_test(free)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
