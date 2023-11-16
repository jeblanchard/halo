#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdlib.h>

#include "kernel/memory/physical_mem.c"

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

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_teardown(test_init_phys_mem,
            teardown_test_init_phys_mem)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}