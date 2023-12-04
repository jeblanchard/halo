#include "kernel/utils/errors.h"
#include "kernel/utils/standard.h"
#include "kernel/utils/memory.h"
#include "kernel/drivers/vesa-display/vesa_display.h"
#include "stdio.h"
#include "kernel/memory/manager.h"
#include "physical_mem.h"

#define NUM_MEMORY_BLOCKS MAX_MEM_ADDR_32_BIT / BYTES_PER_MEMORY_BLOCK
#define BITS_PER_MEMORY_MAP_SECTION 32
#define NUM_MEMORY_MAP_SECTIONS NUM_MEMORY_BLOCKS / BITS_PER_MEMORY_MAP_SECTION

unsigned int mem_map[NUM_MEMORY_MAP_SECTIONS];

#define BYTES_PER_KB 1024

static unsigned int num_accessible_memory_blocks;

unsigned int get_mem_map_section(unsigned int block_num) {
    return block_num / BITS_PER_MEMORY_MAP_SECTION - 1;
}

unsigned int get_mem_map_section_offset_mask(unsigned int block_num) {
    unsigned int block_mask = 1 << (block_num % BITS_PER_MEMORY_MAP_SECTION);
    return block_mask;
}

void clear_mem_block(unsigned int block_num) {
    unsigned int map_section = get_mem_map_section(block_num);
    unsigned int block_mask = get_mem_map_section_offset_mask(block_num);

    mem_map[map_section] &= ~ block_mask;
}

static unsigned int num_blocks_in_use;

unsigned int get_num_blocks_in_use() {
    return num_blocks_in_use;
}

void free_mem_blocks_of_addr_range(physical_address base_addr,
                                      unsigned int length) {

    unsigned int end_addr = base_addr + length;

	unsigned int start_block_num = base_addr / BYTES_PER_MEMORY_BLOCK;
	unsigned int end_block_num = end_addr / BYTES_PER_MEMORY_BLOCK - 1;

    for (unsigned int b = start_block_num; b <= end_block_num; b++) {
        clear_mem_block(b);
        num_blocks_in_use--;
    }
}

void set_memory_block(unsigned int block_num) {
    unsigned int block_section = get_mem_map_section(block_num);
    unsigned int block_mask = get_mem_map_section_offset_mask(block_num);

    if ((mem_map[block_section] & block_mask) == 0) {
        num_blocks_in_use += 1;
    }

    mem_map[block_section] |= block_mask;
}

void set_mem_blocks_of_address_range(physical_address base_addr,
                                     unsigned int range_length) {

    unsigned int end_addr = base_addr + range_length - 1;

	unsigned int start_block_num = base_addr / BYTES_PER_MEMORY_BLOCK;
	unsigned int end_block_num = end_addr / BYTES_PER_MEMORY_BLOCK;

    for (unsigned int b = start_block_num; b <= end_block_num; b++) {
        set_memory_block(b);
    }
}

void classify_mem_region(mem_map_entry* entry) {
    unsigned int entry_base_addr = entry -> base_addr_low;
    unsigned int region_length = entry -> length_in_bytes_low;

    if (entry -> type == AVAILABLE_RAM) {
        free_mem_blocks_of_addr_range(entry_base_addr, region_length);
    } else {
        set_mem_blocks_of_address_range(entry_base_addr, region_length);
    }

}

void config_mem_regions(boot_info* boot_info) {

    unsigned int num_mem_map_entries = boot_info -> mem_map_num_entries;

    mem_map_entry* mem_map_entry_list_base_addr = boot_info -> mem_map_entry_list_base_addr;

    for (unsigned int i = 0; i < num_mem_map_entries; i++) {
        mem_map_entry* entry = mem_map_entry_list_base_addr + i;

        classify_mem_region(entry);
    }
}

void set_all_mem_blocks() {
    for (unsigned int i = 0; i < NUM_MEMORY_MAP_SECTIONS; i++) {
        mem_map[i] = 0xffffffff;
    }

    num_blocks_in_use = num_accessible_memory_blocks;
}

#define BYTES_PER_KB 1024

void config_phys_mem(boot_info* boot_info) {

	unsigned int num_kb_in_mem = boot_info -> num_kb_in_mem;
    unsigned int kb_per_mem_block = BYTES_PER_MEMORY_BLOCK / BYTES_PER_KB;
	num_accessible_memory_blocks = num_kb_in_mem / kb_per_mem_block;

    set_all_mem_blocks();
	config_mem_regions(boot_info);
}

unsigned int get_num_mem_map_sections() {
    return num_accessible_memory_blocks / BITS_PER_MEMORY_MAP_SECTION + 1;
}

bool block_is_set(unsigned int block_num) {
    unsigned int block_section = get_mem_map_section(block_num);
    unsigned int block_mask = get_mem_map_section_offset_mask(block_num);

    unsigned int result = mem_map[block_section] & block_mask;

    if (result != 0) {
        return true;
    }

    return false;
}

#define BLOCKS_PER_BYTE 8

#define MEMORY_MAP_SECTION_MAX_VALUE 0xffffffff

bool mem_map_section_has_space(unsigned int section_num) {
    if (mem_map[section_num] != MEMORY_MAP_SECTION_MAX_VALUE) {
        return true;
    }

    return false;
}

bool offset_in_section_is_free(unsigned int section_num, unsigned int offset) {
    unsigned int block_mask = 1 << offset;

    if ((mem_map[section_num] & block_mask) != 0) {
        return false;
    }

    return true;
}

typedef enum first_free_block_offset_status {
    FIRST_FREE_BLOCK_OFFSET_SUCCESS = 0,
    COULD_NOT_FIND_OFFSET = 1
} first_free_block_offset_status;

typedef struct first_free_block_offset_resp {
    first_free_block_offset_status status;
    unsigned int offset;
} first_free_block_offset_resp;

first_free_block_offset_resp get_offset_of_first_free_block_in_mem_map_section(unsigned int section_num) {

    unsigned int offset = 0;
    bool could_not_find_offset = true;
    for (; offset < BITS_PER_MEMORY_MAP_SECTION; offset++) {
        if (offset_in_section_is_free(section_num, offset)) {
            could_not_find_offset = false;
            break;
        }
    }

    if (could_not_find_offset) {
        return (first_free_block_offset_resp) {
            status: COULD_NOT_FIND_OFFSET,
            offset: 0
        };
    }

    return (first_free_block_offset_resp) {
            status: FIRST_FREE_BLOCK_OFFSET_SUCCESS,
            offset: offset};
}

typedef enum get_first_free_block_num_status {
    GET_FIRST_FREE_BLOCK_NUM_SUCCESS = 0,
    NO_FREE_MEM_BLOCKS = 1,
    FAILED_GETTING_SECTION_OFFSET = 2
} get_first_free_block_num_status;

typedef struct get_first_free_block_num_resp {
    get_first_free_block_num_status status;
    unsigned int block_num;
} get_first_free_block_num_resp;

get_first_free_block_num_resp get_first_free_block_num() {
    bool free_block_dne = true;
    unsigned int num_mem_map_sections = get_num_mem_map_sections();
    unsigned int section_num;
    first_free_block_offset_resp section_offset_resp;
	for (unsigned int s = 0; s < num_mem_map_sections; s++) {
		if (mem_map_section_has_space(s)) {
		    section_offset_resp = get_offset_of_first_free_block_in_mem_map_section(s);
            if (section_offset_resp.status == FIRST_FREE_BLOCK_OFFSET_SUCCESS) {        
            } else {
                return (get_first_free_block_num_resp) {
                    status: FAILED_GETTING_SECTION_OFFSET,
                    block_num: 0};
            }

		    section_num = s;

            free_block_dne = false;
		    break;
		}
	}

	if (free_block_dne) {
        return (get_first_free_block_num_resp) {status: NO_FREE_BLOCKS, block_num: 0};
	}

    unsigned int block_num = section_num * BITS_PER_MEMORY_MAP_SECTION \
        + section_offset_resp.offset;

	return (get_first_free_block_num_resp) {status: GET_FIRST_FREE_BLOCK_NUM_SUCCESS, block_num: block_num};
}

unsigned int get_num_free_blocks() {
    return num_accessible_memory_blocks - num_blocks_in_use;
}

typedef enum get_next_free_frame_status {
    GET_NEXT_FREE_FRAME_SUCCESS = 0,
    COULD_NOT_GET_FIRST_FREE_BLOCK = 1
} get_next_free_frame_status;

typedef struct get_next_free_frame_resp {
    get_next_free_frame_status status;
    physical_address next_free_frame;
} get_next_free_frame_resp;

get_next_free_frame_resp get_next_free_frame() {
    get_first_free_block_num_resp free_block_num_resp = get_first_free_block_num();
    if (free_block_num_resp.status == GET_FIRST_FREE_BLOCK_NUM_SUCCESS) {
    } else {
        return (get_next_free_frame_resp) {
            status: COULD_NOT_GET_FIRST_FREE_BLOCK,
            next_free_frame: 0};
    }

    return (get_next_free_frame_resp) {
        status: GET_NEXT_FREE_FRAME_SUCCESS,
        next_free_frame: BYTES_PER_MEMORY_BLOCK * free_block_num_resp.block_num};
}

bool frame_dne(physical_address frame_base) {
    if (frame_base % BYTES_PER_MEMORY_BLOCK != 0) {
        return true;
    }

    return false;
}

bool frame_is_in_use(physical_address frame_base) {
    unsigned int block_num = frame_base / BYTES_PER_MEMORY_BLOCK;
    return block_is_set(block_num);
}

alloc_spec_frame_resp alloc_spec_frame(physical_address frame_base) {
    if (frame_dne(frame_base)) {
        return (alloc_spec_frame_resp) {
            status: ALLOC_SPEC_FRAME_BASE_DNE,
            buffer_size: 0,
            buffer: NULL};
    }

    if (frame_is_in_use(frame_base)) {
        return (alloc_spec_frame_resp) {
            status: ALLOC_SPEC_FRAME_IN_USE,
            buffer_size: 0,
            buffer: NULL};
    }

    unsigned int block_num = frame_base / BYTES_PER_MEMORY_BLOCK;
	set_memory_block(block_num);

    return (alloc_spec_frame_resp) {
        status: ALLOC_SPEC_FRAME_SUCCESS,
        buffer_size: BYTES_PER_MEMORY_BLOCK,
        buffer: (void*) frame_base};
}

alloc_block_resp alloc_block() {
    get_next_free_frame_resp next_free_frame_resp = get_next_free_frame();
    if (next_free_frame_resp.status == GET_NEXT_FREE_FRAME_SUCCESS) {
    } else {
        return (alloc_block_resp) {
            status: COULD_NOT_GET_FREE_FRAME,
            buffer_size: 0,
            buffer: 0};
    }

    alloc_spec_frame_resp spec_frame_resp = alloc_spec_frame(next_free_frame_resp.next_free_frame);
    if (spec_frame_resp.status == ALLOC_SPEC_FRAME_SUCCESS) {
    } else {
        return (alloc_block_resp) {
            status: COULD_NOT_ALLOC_SPEC_FRAME,
            buffer_size: 0,
            buffer: 0};
    }

    return (alloc_block_resp) {status: ALLOC_BLOCK_SUCCESS,
                               buffer: spec_frame_resp.buffer,
                               buffer_size: BYTES_PER_MEMORY_UNIT};
}

void free_block(physical_address block_address) {
    free_mem_blocks_of_addr_range(block_address, BYTES_PER_MEMORY_BLOCK);
}

void clear_phys_mem_config() {
    set_all_mem_blocks();
}
