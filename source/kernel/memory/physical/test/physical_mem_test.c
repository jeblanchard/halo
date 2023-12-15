#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include "cmocka.h"
#include <stdio.h>
#include <stdlib.h>

#include "kernel/memory/physical/physical_mem.h"

#define BYTES_PER_KB 1024
#define FOUR_GB 0x100000000
#define TYPICAL_NUM_BYTES_IN_MEM FOUR_GB
#define TYPICAL_NUM_KB_IN_MEM TYPICAL_NUM_BYTES_IN_MEM / BYTES_PER_KB

#define NUM_MEM_MAP_ENTRIES 4
#define MEM_MAP_ENTRY_LEN_IN_BYTES \
    TYPICAL_NUM_BYTES_IN_MEM / NUM_MEM_MAP_ENTRIES

mem_map_entry typical_mem_map[NUM_MEM_MAP_ENTRIES] = {

    {base_addr_low: 0,
        base_addr_high: 0,
        length_in_bytes_low: MEM_MAP_ENTRY_LEN_IN_BYTES,
        length_in_bytes_high: 0,
        type: AVAILABLE_TO_OS},

    {base_addr_low: MEM_MAP_ENTRY_LEN_IN_BYTES, 
        base_addr_high: 0,
        length_in_bytes_low: MEM_MAP_ENTRY_LEN_IN_BYTES,
        length_in_bytes_high: 0,
        type: SYSTEM_RESERVED},

    {base_addr_low: 2 * MEM_MAP_ENTRY_LEN_IN_BYTES,
        base_addr_high: 0,
        length_in_bytes_low: MEM_MAP_ENTRY_LEN_IN_BYTES,
        length_in_bytes_high: 0,
        type: ACPI_NVS},

    {base_addr_low: 3 * MEM_MAP_ENTRY_LEN_IN_BYTES,
        base_addr_high: 0,
        length_in_bytes_low: MEM_MAP_ENTRY_LEN_IN_BYTES,
        length_in_bytes_high: 0,
        type: ACPI_NVS}
};

boot_info typical_boot_info = {
    num_kb_in_mem: TYPICAL_NUM_KB_IN_MEM,
    mem_map_entry_list_base_addr: typical_mem_map,
    mem_map_num_entries: NUM_MEM_MAP_ENTRIES,
};

typedef enum get_io_addr_status {
    GET_IO_ADDR_SUCCESS = 0,
    NO_IO_REGION = 1
} get_io_addr_status;

typedef struct get_io_addr_resp {
    get_io_addr_status status;
    physical_address io_addr;
} get_io_addr_resp;

get_io_addr_resp get_io_frame_base_addr(boot_info some_boot_info) {
    mem_map_entry* some_mem_map = (mem_map_entry*) some_boot_info.mem_map_entry_list_base_addr;
    for (unsigned int i = 0; i < some_boot_info.mem_map_num_entries; i++) {
        mem_map_entry curr_entry = some_mem_map[i];
        if (curr_entry.type == SYSTEM_RESERVED) {
            physical_address some_io_addr = curr_entry.base_addr_low;
            return (get_io_addr_resp) {
                status: GET_IO_ADDR_SUCCESS,
                io_addr: some_io_addr
            };
        }
    }

    return (get_io_addr_resp) {
        status: NO_IO_REGION,
        io_addr: 0
    };
}

typedef enum get_avail_to_os_addr_status {
    GET_AVAIL_TO_OS_ADDR_SUCCESS = 0,
    NO_AVAIL_TO_OS_REGION = 1
} get_avail_to_os_addr_status;

typedef struct get_avail_to_os_addr_resp {
    get_avail_to_os_addr_status status;
    physical_address avail_to_os_addr;
} get_avail_to_os_addr_resp;

get_avail_to_os_addr_resp get_avail_to_os_frame_base_addr(boot_info some_boot_info) {
    mem_map_entry* some_mem_map = (mem_map_entry*) some_boot_info.mem_map_entry_list_base_addr;
    for (unsigned int i = 0; i < some_boot_info.mem_map_num_entries; i++) {
        mem_map_entry curr_entry = some_mem_map[i];
        if (curr_entry.type == AVAILABLE_TO_OS) {
            physical_address some_avail_to_os_addr = \
                curr_entry.base_addr_low;
            return (get_avail_to_os_addr_resp) {
                status: GET_IO_ADDR_SUCCESS,
                avail_to_os_addr: some_avail_to_os_addr
            };
        }
    }

    return (get_avail_to_os_addr_resp) {
        status: NO_AVAIL_TO_OS_REGION,
        avail_to_os_addr: 0
    };
}

static void is_io_addr_true_test(void **state) {
    (void) state;

    get_io_addr_resp some_io_addr_resp = get_io_frame_base_addr(typical_boot_info);
    assert_true(some_io_addr_resp.status == GET_IO_ADDR_SUCCESS);

    config_phys_mem(&typical_boot_info);

    mem_mapped_io_resp actual_resp = frame_has_mem_mapped_io(some_io_addr_resp.io_addr);
    assert_true(actual_resp.status == MEM_MAPPED_IO_SUCCESS);

    assert_true(actual_resp.has_mem_mapped_io);
}

static int is_io_addr_true_test_teardown(void **state) {
    (void) state;

    clear_phys_mem_config();

    return 0;
}

static void is_io_addr_false_test(void **state) {
    (void) state;

    get_avail_to_os_addr_resp non_io_addr_resp = get_avail_to_os_frame_base_addr(typical_boot_info);
    assert_true(non_io_addr_resp.status == GET_AVAIL_TO_OS_ADDR_SUCCESS);

    config_phys_mem(&typical_boot_info);

    mem_mapped_io_resp actual_resp = frame_has_mem_mapped_io(non_io_addr_resp.avail_to_os_addr);
    assert_true(actual_resp.status == MEM_MAPPED_IO_SUCCESS);

    assert_false(actual_resp.has_mem_mapped_io);
}

static int is_io_addr_false_test_teardown(void **state) {
    (void) state;

    clear_phys_mem_config();

    return 0;
}

static int config_phys_mem_test_teardown(void **state) {
    (void) state;

    clear_phys_mem_config();

    return 0;
}

static void config_phys_mem_test(void **state) {
    (void) state;

    config_phys_mem(&typical_boot_info);

    unsigned int correct_num_blocks_in_use = \
        3 * ((MEM_MAP_ENTRY_LEN_IN_BYTES) / BYTES_PER_MEMORY_BLOCK);

    assert_true(get_num_blocks_in_use() == correct_num_blocks_in_use);

    unsigned int correct_num_free_blocks = \
        MEM_MAP_ENTRY_LEN_IN_BYTES / BYTES_PER_MEMORY_BLOCK;

    assert_true(get_num_free_blocks() == correct_num_free_blocks);
}

static void clear_phys_mem_config_test(void **state) {
    (void) state;

    config_phys_mem(&typical_boot_info);
    clear_phys_mem_config();

    assert_true(get_num_free_blocks() == 0);
    assert_true(get_num_blocks_in_use() == 
        MAX_MEM_ADDR_32_BIT / BYTES_PER_MEMORY_BLOCK + 1);
}

static void clear_phys_mem_config_needs_config_test(void **state) {
    (void) state;

    assert_true(phys_mem_needs_config());

    config_phys_mem(&typical_boot_info);
    assert_false(phys_mem_needs_config());

    clear_phys_mem_config();
    
    assert_true(phys_mem_needs_config());
}

static void alloc_block_success_test(void **state) {
    (void) state;

    config_phys_mem(&typical_boot_info);

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

static int free_block_test_setup(void **state) {
    (void) state;

    config_phys_mem(&typical_boot_info);

    return 0;
}

static int free_block_test_teardown(void **state) {
    (void) state;

    clear_phys_mem_config();

    return 0;
}

static void free_block_test(void **state) {
    (void) state;

    alloc_block_resp resp_of_block_to_free = alloc_block();
    assert_true(resp_of_block_to_free.status == ALLOC_BLOCK_SUCCESS);

    unsigned int old_num_blocks_in_use = get_num_blocks_in_use();

    free_block((physical_address) resp_of_block_to_free.buffer_base_addr);

    assert_int_equal(old_num_blocks_in_use - 1, get_num_blocks_in_use());
}

static void free_block_duplicate_test(void **state) {
    (void) state;

    alloc_block_resp resp_of_block_to_free = alloc_block();
    assert_true(resp_of_block_to_free.status == ALLOC_BLOCK_SUCCESS);

    free_block(resp_of_block_to_free.buffer_base_addr);
    unsigned int num_blocks_in_use_after_first_free = get_num_blocks_in_use();

    free_block(resp_of_block_to_free.buffer_base_addr);
    unsigned int num_blocks_in_use_after_duplic_free = get_num_blocks_in_use();

    assert_true(num_blocks_in_use_after_first_free == num_blocks_in_use_after_duplic_free);
}

static void get_num_blocks_in_use_test(void **state) {
    (void) state;

    config_phys_mem(&typical_boot_info);

    unsigned int correct_num_blocks_in_use = 
        3 * ((MEM_MAP_ENTRY_LEN_IN_BYTES) / BYTES_PER_MEMORY_BLOCK);
    
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

    config_phys_mem(&typical_boot_info);

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

static void phys_mem_needs_configuring_test(void **state) {
    (void) state;

    assert_true(phys_mem_needs_config());

    config_phys_mem(&typical_boot_info);

    assert_false(phys_mem_needs_config());
}

static int has_phys_mem_been_configured_test_teardown(void **state) {
    (void) state;

    clear_phys_mem_config();

    return 0;
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
        cmocka_unit_test_setup_teardown(free_block_test,
            free_block_test_setup,
            free_block_test_teardown),
        cmocka_unit_test_setup_teardown(free_block_duplicate_test,
            free_block_test_setup,
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
        cmocka_unit_test(clear_phys_mem_config_test),
        cmocka_unit_test_teardown(is_io_addr_true_test,
            is_io_addr_true_test_teardown),
        cmocka_unit_test_teardown(is_io_addr_false_test,
            is_io_addr_false_test_teardown),
        cmocka_unit_test_teardown(phys_mem_needs_configuring_test,
            has_phys_mem_been_configured_test_teardown),
        cmocka_unit_test(clear_phys_mem_config_needs_config_test)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
