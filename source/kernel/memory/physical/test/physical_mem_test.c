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

mem_map_entry fake_mem_map_entry_list[NUM_MEM_MAP_ENTRIES] = {

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

    printf("correct number of blocks in use: %d\n", correct_num_blocks_in_use);
    printf("actual number of blocks in use: %d\n", get_num_blocks_in_use());

    printf("actual number of free blocks: %d\n", get_num_free_blocks());

    assert_true(get_num_blocks_in_use() == correct_num_blocks_in_use);

    unsigned int correct_num_free_blocks = \
        FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES / BYTES_PER_MEMORY_BLOCK;

    assert_true(get_num_free_blocks() == correct_num_free_blocks);

    clear_phys_mem_config();
}

// static void clear_phys_mem_config_test(void **state) {
//     (void) state;

//     config_phys_mem(&fake_boot_info);
//     clear_phys_mem_config();

//     assert_true(get_num_free_blocks() == 0);
//     assert_true(get_num_blocks_in_use() == 
//         MAX_MEM_ADDR_32_BIT / BYTES_PER_MEMORY_BLOCK + 1);

//     printf("actual number of blocks in use: %d\n", get_num_blocks_in_use());
// }

// #define TWENTY_KB 0x5000

// #define PHYSICAL_MEM_SIZE_IN_BYTES TWENTY_KB

// unsigned char fake_physical_mem[PHYSICAL_MEM_SIZE_IN_BYTES];

// static void alloc_block_test(void **state) {
//     (void) state;

//     config_phys_mem(&fake_boot_info);

//     unsigned int old_num_blocks_in_use = get_num_blocks_in_use();

//     alloc_block_resp res = alloc_block();
//     assert_true(res.status == BLOCK_ALLOC_SUCCESS);

//     assert_true(res.buffer_size == BYTES_PER_MEMORY_BLOCK);
//     assert_int_equal(1 + old_num_blocks_in_use, get_num_blocks_in_use());

//     free_block((physical_address) res.buffer);
//     clear_phys_mem_config();

//     printf("actual number of blocks in use: %d\n", get_num_blocks_in_use());
// }

// #define TWENTY_KB 0x5000

// #define PHYSICAL_MEM_SIZE_IN_BYTES TWENTY_KB

// unsigned char fake_physical_mem[PHYSICAL_MEM_SIZE_IN_BYTES];

// alloc_block_resp resp_of_block_to_free;

// static void free_block_test(void **state) {
//     (void) state;

//     mem_map_entry all_mem_avail = {
//         base_addr_low: 0,
//         base_addr_high: 0,
//         length_in_bytes_low: PHYSICAL_MEM_SIZE_IN_BYTES,
//         length_in_bytes_high: 0,
//         type: AVAILABLE_RAM
//     };

//     boot_info custom_mem_boot = {
//         num_kb_in_mem: PHYSICAL_MEM_SIZE_IN_BYTES / BYTES_PER_KB,
//         mem_map_entry_list_base_addr: &all_mem_avail,
//         mem_map_num_entries: 1
//     };

//     config_phys_mem(&custom_mem_boot);

//     resp_of_block_to_free = alloc_block();

//     unsigned int old_num_blocks_in_use = get_num_blocks_in_use();

//     free_block((physical_address) resp_of_block_to_free.buffer);

//     assert_int_equal(old_num_blocks_in_use - 1, get_num_blocks_in_use());

//     clear_phys_mem_config();

//     printf("actual number of blocks in use: %d\n", get_num_blocks_in_use());
// }

// static void get_num_blocks_in_use_test(void **state) {
//     (void) state;

//     config_phys_mem(&fake_boot_info);

//     unsigned int correct_num_blocks_in_use = 
//         3 * ((FAKE_MEM_MAP_ENTRY_LENGTH_IN_BYTES) / BYTES_PER_MEMORY_BLOCK);
    
//     assert_true(get_num_blocks_in_use() == correct_num_blocks_in_use);
//     printf("correct number of blocks in use: %d\n", correct_num_blocks_in_use);
//     printf("actual number of blocks in use: %d\n", get_num_blocks_in_use());

//     clear_phys_mem_config();

//     printf("actual number of blocks in use: %d\n", get_num_blocks_in_use());
// }

// static int alloc_spec_frame_success_teardown(void **state) {
//     (void) state;

//     clear_phys_mem_config();

//     return 0;
// }

// static void alloc_spec_frame_success_test(void **state) {
//     (void) state;

//     config_phys_mem(&fake_boot_info);

//     #define THREE_GB 0xc0000000
//     #define FOUR_KB_FRAME_BASE THREE_GB

//     unsigned int og_num_blocks_in_use = get_num_blocks_in_use();
//     printf("number of blocks in use: %d\n", og_num_blocks_in_use);
//     printf("number of free blocks: %d\n", get_num_free_blocks());

//     alloc_spec_frame_resp frame_resp = alloc_spec_frame(FOUR_KB_FRAME_BASE);

//     printf("frame response status: %d\n", frame_resp.status);
//     assert_true(frame_resp.status == ALLOC_SPEC_FRAME_SUCCESS);
//     assert_true(get_num_blocks_in_use() == og_num_blocks_in_use + 1);

//     free_block((physical_address) frame_resp.buffer);

//     printf("actual number of blocks in use: %d\n", get_num_blocks_in_use());
// }

// static int alloc_spec_frame_in_use_teardown(void **state) {
//     (void) state;

//     clear_phys_mem_config();

//     return 0;
// }

// static void alloc_spec_frame_in_use_test(void **state) {
//     (void) state;

//     #define TWO_MB 0x20000
//     #define THREE_GB 0xc0000000
//     physical_address frame = THREE_GB + TWO_MB;

//     alloc_spec_frame_resp frame_resp = alloc_spec_frame(frame);
//     assert_true(frame_resp.status == ALLOC_SPEC_FRAME_SUCCESS);

//     alloc_spec_frame_resp repeat_frame_resp = alloc_spec_frame(frame);
//     assert_true(repeat_frame_resp.status == ALLOC_SPEC_FRAME_IN_USE);

//     printf("actual number of blocks in use: %d\n", get_num_blocks_in_use());
// }

// static int alloc_spec_frame_dne_teardown(void **state) {
//     (void) state;

//     clear_phys_mem_config();

//     return 0;
// }

// static void alloc_spec_frame_dne_test(void **state) {
//     (void) state;

//     #define THREE_GB 0xc0000000
//     #define FOUR_KB_FRAME_BASE THREE_GB
//     #define FRAME_OFFSET 5
//     physical_address uneven_frame = FOUR_KB_FRAME_BASE + FRAME_OFFSET;

//     alloc_spec_frame_resp uneven_frame_resp = alloc_spec_frame(uneven_frame);
//     assert_true(uneven_frame_resp.status == ALLOC_SPEC_FRAME_BASE_DNE);

//     alloc_spec_frame_resp good_frame_resp = alloc_spec_frame(FOUR_KB_FRAME_BASE);
//     assert_true(good_frame_resp.status == ALLOC_SPEC_FRAME_SUCCESS);

//     printf("actual number of blocks in use: %d\n", get_num_blocks_in_use());
// }

int main() {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test_teardown(config_phys_mem_test,
            config_phys_mem_test_teardown)
        cmocka_unit_test(get_num_blocks_in_use_test),
        cmocka_unit_test(alloc_block_test),
        cmocka_unit_test(free_block_test),
        cmocka_unit_test_teardown(alloc_spec_frame_success_test,
            alloc_spec_frame_success_teardown),
        cmocka_unit_test_teardown(alloc_spec_frame_in_use_test,
            alloc_spec_frame_in_use_teardown),
        cmocka_unit_test_teardown(alloc_spec_frame_dne_test,
            alloc_spec_frame_dne_teardown),
        cmocka_unit_test(clear_phys_mem_config_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
