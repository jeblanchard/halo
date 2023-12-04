#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdlib.h>

#include "kernel/memory/physical/physical_mem.h"

#define NUM_MEM_MAP_ENTRIES 4
#define FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES \
    ((MAX_MEM_ADDR_32_BIT / NUM_MEM_MAP_ENTRIES) + 1)

mem_map_entry mem_map_entry_list[NUM_MEM_MAP_ENTRIES] = {

    {base_addr_low: 0,
        base_addr_high: 0,
        length_in_bytes_low: FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES,
        length_in_bytes_high: 0,
        type: AVAILABLE_RAM},

    {base_addr_low: FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES, 
        base_addr_high: 0,
        length_in_bytes_low: FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES,
        length_in_bytes_high: 0,
        type: RESERVED_MEMORY},

    {base_addr_low: 2 * FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES,
        base_addr_high: 0,
        length_in_bytes_low: FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES,
        length_in_bytes_high: 0,
        type: RESERVED_MEMORY},

    {base_addr_low: 3 * FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES,
        base_addr_high: 0,
        length_in_bytes_low: FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES,
        length_in_bytes_high: 0,
        type: RESERVED_MEMORY}
};

#define BYTES_PER_KB 1024

boot_info fake_boot_info = {
    num_kb_in_mem: (MAX_MEM_ADDR_32_BIT / BYTES_PER_KB) + 1,
    mem_map_entry_list_base_addr: &fake_mem_map_entry_list[0],
    mem_map_num_entries: NUM_MEM_MAP_ENTRIES,
};

static int config_phys_mem_test_teardown(void **state) {
    (void) state;

    clear_phys_mem_config();

    return 0;
}

static void config_phys_mem_test(void **state) {
    (void) state;

    config_phys_mem(&fake_boot_info);

    unsigned int correct_num_blocks_in_use = \
        3 * ((FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES) / BYTES_PER_MEMORY_BLOCK);

    assert_true(get_num_blocks_in_use() == correct_num_blocks_in_use);

    unsigned int correct_num_free_blocks = \
        FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES / BYTES_PER_MEMORY_BLOCK;

    assert_true(get_num_free_blocks() == correct_num_free_blocks);
}

static void clear_phys_mem_config_test(void **state) {
    (void) state;

    config_phys_mem(&fake_boot_info);
    clear_phys_mem_config();

    assert_true(get_num_free_blocks() == 0);
    assert_true(get_num_blocks_in_use() == 
        MAX_MEM_ADDR_32_BIT / BYTES_PER_MEMORY_BLOCK + 1);
}

static void alloc_block_success_test(void **state) {
    (void) state;

    config_phys_mem(&fake_boot_info);

    unsigned int old_num_blocks_in_use = get_num_blocks_in_use();

    alloc_block_resp res = alloc_block();
    assert_true(res.status == ALLOC_BLOCK_SUCCESS);

    assert_true(res.buffer_size == BYTES_PER_MEMORY_BLOCK);
    assert_int_equal(1 + old_num_blocks_in_use, get_num_blocks_in_use());
}

static int alloc_block_success_teardown(void **state) {
    (void) state;

    clear_phys_mem_config();

    return 0;
}

static void free_block_test(void **state) {
    (void) state;

    config_phys_mem(&fake_boot_info);

    alloc_block_resp resp_of_block_to_free = alloc_block();
    assert_true(resp_of_block_to_free.status == ALLOC_BLOCK_SUCCESS);

    unsigned int old_num_blocks_in_use = get_num_blocks_in_use();

    free_block((physical_address) resp_of_block_to_free.buffer);

    assert_int_equal(old_num_blocks_in_use - 1, get_num_blocks_in_use());
}

static int free_block_test_teardown(void **state) {
    (void) state;

    clear_phys_mem_config();

    return 0;
}

static void get_num_blocks_in_use_test(void **state) {
    (void) state;

    config_phys_mem(&fake_boot_info);

    unsigned int correct_num_blocks_in_use = 
        3 * ((FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES) / BYTES_PER_MEMORY_BLOCK);
    
    assert_true(get_num_blocks_in_use() == correct_num_blocks_in_use);
}

static int get_num_blocks_in_use_test_teardown(void **state) {
    (void) state;

    clear_phys_mem_config();

    return 0;
}

static int alloc_spec_frame_test_teardown(void **state) {
    (void) state;

    clear_phys_mem_config();

    return 0;
}

#define ALIGNED_FRAME_BASE_ADDR 3 * BYTES_PER_MEMORY_BLOCK

static int alloc_spec_frame_test_setup(void **state) {
    (void) state;

    config_phys_mem(&fake_boot_info);

    return 0;
}

static void alloc_spec_frame_success_test(void **state) {
    (void) state;

    unsigned int og_num_blocks_in_use = get_num_blocks_in_use();

    alloc_spec_frame_resp frame_resp = alloc_spec_frame(ALIGNED_FRAME_BASE_ADDR);

    assert_true(frame_resp.status == ALLOC_SPEC_FRAME_SUCCESS);
    assert_true(get_num_blocks_in_use() == og_num_blocks_in_use + 1);
}

static void alloc_spec_frame_in_use_test(void **state) {
    (void) state;

    alloc_spec_frame_resp frame_resp = alloc_spec_frame(ALIGNED_FRAME_BASE_ADDR);
    assert_true(frame_resp.status == ALLOC_SPEC_FRAME_SUCCESS);

    alloc_spec_frame_resp repeat_frame_resp = alloc_spec_frame(ALIGNED_FRAME_BASE_ADDR);
    assert_true(repeat_frame_resp.status == ALLOC_SPEC_FRAME_IN_USE);
}

static void alloc_spec_frame_dne_test(void **state) {
    (void) state;

    physical_address uneven_frame = ALIGNED_FRAME_BASE_ADDR + 1;

    alloc_spec_frame_resp uneven_frame_resp = alloc_spec_frame(uneven_frame);
    assert_true(uneven_frame_resp.status == ALLOC_SPEC_FRAME_BASE_DNE);
}

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_teardown(config_phys_mem_test,
            config_phys_mem_test_teardown),
        cmocka_unit_test_teardown(get_num_blocks_in_use_test,
            get_num_blocks_in_use_test_teardown),
        cmocka_unit_test_setup_teardown(alloc_block_success_test,
            alloc_spec_frame_test_setup,
            alloc_block_success_teardown),
        cmocka_unit_test_teardown(free_block_test,
            free_block_test_teardown),
        cmocka_unit_test_setup_teardown(alloc_spec_frame_success_test,
            alloc_spec_frame_test_setup,
            alloc_spec_frame_test_teardown),
        cmocka_unit_test_setup_teardown(alloc_spec_frame_in_use_test,
            alloc_spec_frame_test_setup,
            alloc_spec_frame_test_teardown),
        cmocka_unit_test_setup_teardown(alloc_spec_frame_dne_test,
            alloc_spec_frame_test_setup,
            alloc_spec_frame_test_teardown),
        cmocka_unit_test(clear_phys_mem_config_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
