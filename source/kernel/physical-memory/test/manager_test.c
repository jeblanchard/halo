#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>

#include "kernel/physical-memory/manager.c"
#include "kernel/physical-memory/boot.h"

static int teardown_test_init_phys_mem_manager(void **state) {
    (void) state;

    num_blocks_in_use = 0;

    return 0;
}

#define FAKE_NUM_KB_IN_MEM 1000

#define CORRECT_NUM_ACCESSIBLE_BLOCKS (FAKE_NUM_KB_IN_MEM * BYTES_PER_KB) / BYTES_PER_MEMORY_BLOCK

static void test_init_phys_mem_manager(void **state) {
    (void) state;

    #define NUM_MEM_MAP_ENTRIES 4

    memory_map_entry fake_mem_map_entry_list[NUM_MEM_MAP_ENTRIES] = {

        {base_addr_low: 0, base_addr_high: 0,
         length_low: 0, length_high: 0,
         type: 1},

        {base_addr_low: 0, base_addr_high: 0,
         length_low: 0, length_high: 0,
         type: 1},

         {base_addr_low: 0, base_addr_high: 0,
         length_low: 0, length_high: 0,
         type: 1},

         {base_addr_low: 0, base_addr_high: 0,
         length_low: 0, length_high: 0,
         type: 1}
    };

    multiboot2_info fake_boot_info = {
        total_size: 1000,
        reserved: 0,
        mem_info_type: 0,
        mem_info_size: 0,
        num_kb_in_mem: FAKE_NUM_KB_IN_MEM,
        mem_map_info_type: 1,
        mem_map_info_size: 1,
        entry_size: 1,
        entry_version: 0,
        &fake_mem_map_entry_list[0],
        NUM_MEM_MAP_ENTRIES,
        terminating_tag_type: 1,
        terminating_tag_size: 1
    };

    init_phys_mem_manager(&fake_boot_info);

    printf("%d\n", num_blocks_in_use);
    printf("%d\n", CORRECT_NUM_ACCESSIBLE_BLOCKS);
    
    assert_true(num_blocks_in_use == CORRECT_NUM_ACCESSIBLE_BLOCKS);

    for (unsigned int i = 0; i < sizeof(memory_map); i++) {
        assert_true(memory_map[i] == 0xff);
    }
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_teardown(test_init_phys_mem_manager,
            teardown_test_init_phys_mem_manager),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
