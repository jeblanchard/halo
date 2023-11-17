#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdlib.h>

#include "kernel/memory/physical/physical_mem.c"

static int teardown_test_init_phys_mem(void **state) {
    (void) state;

    num_blocks_in_use = 0;

    unsigned int num_bytes_to_null_out = sizeof(memory_map);
    set_memory(memory_map, 0, num_bytes_to_null_out);

    return 0;
}

#define FAKE_NUM_BYTES_IN_MEM 1000 * 1000

#define NUM_MEM_MAP_ENTRIES 4

static void test_init_phys_mem(void **state) {
    (void) state;

    unsigned int mem_range_size_in_bytes = FAKE_NUM_BYTES_IN_MEM / NUM_MEM_MAP_ENTRIES;

    mem_map_entry fake_mem_map_entry_list[NUM_MEM_MAP_ENTRIES] = {

        {base_addr_low: 0,
         base_addr_high: 0,
         length_in_bytes_low: mem_range_size_in_bytes,
         length_in_bytes_high: 0,
         type: AVAILABLE_RAM},

        {base_addr_low: mem_range_size_in_bytes, 
         base_addr_high: 0,
         length_in_bytes_low: mem_range_size_in_bytes,
         length_in_bytes_high: 0,
         type: RESERVED_MEMORY},

        {base_addr_low: 2 * mem_range_size_in_bytes,
         base_addr_high: 0,
         length_in_bytes_low: mem_range_size_in_bytes,
         length_in_bytes_high: 0,
         type: RESERVED_MEMORY},

        {base_addr_low: 3 * mem_range_size_in_bytes,
         base_addr_high: 0,
         length_in_bytes_low: mem_range_size_in_bytes,
         length_in_bytes_high: 0,
         type: RESERVED_MEMORY}
    };

    boot_info fake_boot_info = {
        num_kb_in_mem: FAKE_NUM_BYTES_IN_MEM / BYTES_PER_KB,
        mem_map_entry_list_base_addr: &fake_mem_map_entry_list[0],
        mem_map_num_entries: NUM_MEM_MAP_ENTRIES,
    };

    init_phys_mem(&fake_boot_info);

    unsigned int correct_num_blocks_in_use = \
        (NUM_MEM_MAP_ENTRIES - 1) * mem_range_size_in_bytes / BYTES_PER_MEMORY_BLOCK;

    assert_true(num_blocks_in_use == correct_num_blocks_in_use);
}

#define TWENTY_KB 0x5000

#define PHYSICAL_MEM_SIZE_IN_BYTES TWENTY_KB

unsigned char fake_physical_mem[PHYSICAL_MEM_SIZE_IN_BYTES];

static int setup_alloc_block_test(void **state) {
    (void) state;
    
    mem_map_entry all_mem_avail = {
        base_addr_low: 0,
        base_addr_high: 0,
        length_in_bytes_low: PHYSICAL_MEM_SIZE_IN_BYTES,
        length_in_bytes_high: 0,
        type: AVAILABLE_RAM
    };

    boot_info custom_mem_boot = {
        num_kb_in_mem: PHYSICAL_MEM_SIZE_IN_BYTES / BYTES_PER_KB,
        mem_map_entry_list_base_addr: &all_mem_avail,
        mem_map_num_entries: 1
    };

    init_phys_mem(&custom_mem_boot);

    return 0;
}

static void alloc_block_test(void **state) {
    (void) state;

    unsigned int old_num_blocks_in_use = get_num_blocks_in_use();

    block_alloc_resp res = alloc_block();
    
    assert_true(res.status == BLOCK_ALLOC_SUCCESS);

    assert_int_equal(1 + old_num_blocks_in_use, get_num_blocks_in_use());

    unsigned int fake_physical_mem_index = (unsigned int) res.buffer;
    for (int i = 0; i < res.buffer_size; i++) {
        fake_physical_mem[fake_physical_mem_index + i] = 'a';
    }

    for (int i = 0; i < res.buffer_size; i++) {
        assert_true(fake_physical_mem[i] == 'a');
    }
}

#define TWENTY_KB 0x5000

#define PHYSICAL_MEM_SIZE_IN_BYTES TWENTY_KB

unsigned char fake_physical_mem[PHYSICAL_MEM_SIZE_IN_BYTES];

block_alloc_resp resp_of_block_to_free;

static int setup_free_block_test(void **state) {
    (void) state;
    
    mem_map_entry all_mem_avail = {
        base_addr_low: 0,
        base_addr_high: 0,
        length_in_bytes_low: PHYSICAL_MEM_SIZE_IN_BYTES,
        length_in_bytes_high: 0,
        type: AVAILABLE_RAM
    };

    boot_info custom_mem_boot = {
        num_kb_in_mem: PHYSICAL_MEM_SIZE_IN_BYTES / BYTES_PER_KB,
        mem_map_entry_list_base_addr: &all_mem_avail,
        mem_map_num_entries: 1
    };

    init_phys_mem(&custom_mem_boot);

    resp_of_block_to_free = alloc_block();

    return 0;
}

static void free_block_test(void **state) {
    (void) state;

    unsigned int old_num_blocks_in_use = get_num_blocks_in_use();

    free_block((physical_address) resp_of_block_to_free.buffer);

    assert_int_equal(old_num_blocks_in_use - 1, get_num_blocks_in_use());
}

static int teardown_get_num_blocks_in_use_test(void **state) {
    (void) state;
    num_blocks_in_use = 0;

    return 0;
}

#define FAKE_NUM_BLOCKS_IN_USE 10

static int setup_get_num_blocks_in_use_test(void **state) {
    (void) state;
    num_blocks_in_use = FAKE_NUM_BLOCKS_IN_USE;

    return 0;
}

static void get_num_blocks_in_use_test(void **state) {
    (void) state;

    int actual_num_blocks_in_use = get_num_blocks_in_use();
    
    assert_int_equal(actual_num_blocks_in_use, FAKE_NUM_BLOCKS_IN_USE);
}

void __wrap_load_pdbr_asm(physical_address new_pdbr_base_addr) {
    (void) new_pdbr_base_addr;
}

static void load_pdbr_test(void **state) {
    (void) state;

    physical_address new_pdbr_base_addr = 0x1234;
    load_pdbr(new_pdbr_base_addr);

    assert_true(new_pdbr_base_addr == get_curr_pdbr());
}

static void get_curr_pdbr_test(void **state) {
    (void) state;

    current_pdbr_base_addr = 0x1234;
    assert_true(current_pdbr_base_addr == get_curr_pdbr());
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_teardown(test_init_phys_mem,
            teardown_test_init_phys_mem),
        cmocka_unit_test_setup_teardown(get_num_blocks_in_use_test,
            setup_get_num_blocks_in_use_test,
            teardown_get_num_blocks_in_use_test),
        cmocka_unit_test_setup(alloc_block_test,
            setup_alloc_block_test),
        cmocka_unit_test_setup(free_block_test,
            setup_free_block_test),
        cmocka_unit_test(load_pdbr_test),
        cmocka_unit_test(get_curr_pdbr_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
