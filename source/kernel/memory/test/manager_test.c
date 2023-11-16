#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "kernel/memory/manager.c"
#include "kernel/memory/boot.h"

#define FAKE_BYTES_PER_MEM_BLOCK 4096

block_alloc_resp __wrap_alloc_block() {
    block_alloc_resp* resp = mock_ptr_type(block_alloc_resp*);
    return *resp;
}


#define NUM_BYTES_HALLOCED 20
char fake_physical_mem_block[NUM_BYTES_HALLOCED];

block_alloc_resp block_alloc_success_resp = \
    {status: BLOCK_ALLOC_SUCCESS, buffer: &fake_physical_mem_block};

static void alloc_page(void **state) {
    (void) state;

    will_return(__wrap_alloc_block, (uintptr_t) &block_alloc_success_resp);

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

block_alloc_resp no_free_blocks_resp = {status: NO_FREE_BLOCKS, buffer: NULL};

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
        cmocka_unit_test(alloc_page),
        cmocka_unit_test(halloc_not_enough_mem),
        cmocka_unit_test(free)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
