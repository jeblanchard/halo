#include "kernel/utils/errors.h"
#include "kernel/utils/standard.h"
#include "kernel/utils/memory.h"
#include "kernel/drivers/vesa-display/vesa_display.h"
#include "stdio.h"
#include "kernel/memory/manager.h"
#include "phys_mem_mgr.h"

#define NUM_MEMORY_BLOCKS (MAX_MEM_ADDR_32_BIT / BYTES_PER_MEMORY_BLOCK + 1)
#define BITS_PER_MEMORY_MAP_SECTION 32
#define NUM_MEMORY_MAP_SECTIONS NUM_MEMORY_BLOCKS / BITS_PER_MEMORY_MAP_SECTION

unsigned int mem_map[NUM_MEMORY_MAP_SECTIONS];

#define BYTES_PER_KB 1024

static unsigned int num_accessible_memory_blocks;

unsigned int get_mem_map_section(unsigned int block_num) {
    return block_num / BITS_PER_MEMORY_MAP_SECTION;
}

unsigned int get_mem_map_section_offset_mask(unsigned int block_num) {
    unsigned int block_mask = 1 << (block_num % BITS_PER_MEMORY_MAP_SECTION);
    return block_mask;
}

static unsigned int num_blocks_in_use;

unsigned int get_num_blocks_in_use() {
    return num_blocks_in_use;
}

void clear_mem_block(unsigned int block_num) {
    unsigned int map_section = get_mem_map_section(block_num);
    unsigned int block_mask = get_mem_map_section_offset_mask(block_num);

    if (mem_map[map_section] & block_mask) {
        num_blocks_in_use -= 1;
    }

    mem_map[map_section] &= ~block_mask;
}

typedef struct mem_block_range {
    unsigned int start_block_num;
    unsigned int last_block_num;
} mem_block_range;

mem_block_range get_mem_block_range(physical_address base_addr,
                                    unsigned int length) {

    unsigned int end_addr = base_addr + (length - 1);

    return (mem_block_range) {
        start_block_num: base_addr / BYTES_PER_MEMORY_BLOCK,
        last_block_num: end_addr / BYTES_PER_MEMORY_BLOCK
    };
}

void free_mem_blocks_of_addr_range(physical_address base_addr,
                                   unsigned int length) {

    mem_block_range range_to_clear = get_mem_block_range(base_addr, length);
    for (unsigned int b = range_to_clear.start_block_num;
         b <= range_to_clear.last_block_num; b++) {
        clear_mem_block(b);
    }
}

void set_memory_block(unsigned int block_num) {
    unsigned int map_section = get_mem_map_section(block_num);
    unsigned int block_mask = get_mem_map_section_offset_mask(block_num);

    if ((mem_map[map_section] & block_mask) == 0) {
        num_blocks_in_use += 1;
    }

    mem_map[map_section] |= block_mask;
}

void set_mem_blocks_of_address_range(physical_address base_addr,
                                     unsigned int length) {

    mem_block_range range_to_clear = get_mem_block_range(base_addr, length);
    for (unsigned int b = range_to_clear.start_block_num;
         b <= range_to_clear.last_block_num;
         b++) {
            
        set_memory_block(b);
    }
}

typedef struct addr_range {
    physical_address base_addr;
    physical_address end_addr_incl;
} addr_range;

#define MAX_NUM_POTENTIAL_IO_RANGES 50
addr_range all_potential_io_ranges[MAX_NUM_POTENTIAL_IO_RANGES];
unsigned int next_io_range_index = 0;

bool ranges_overlap(addr_range range_a, addr_range range_b) {
    if (range_b.base_addr >= range_a.base_addr &&
        range_b.base_addr <= range_a.end_addr_incl) {
        
        return true;
    }

    if (range_a.base_addr >= range_b.base_addr &&
        range_a.base_addr <= range_b.end_addr_incl) {

        return true;
    }

    return false;
}

addr_range new_addr_range(physical_address range_base_addr, unsigned int range_length) {
    return (addr_range) {
        base_addr: range_base_addr,
        end_addr_incl: range_base_addr + range_length - 1
    };
}

bool frame_base_dne(physical_address frame_base) {
    if (frame_base % BYTES_PER_MEMORY_BLOCK != 0) {
        return true;
    }

    return false;
}

mem_mapped_io_resp frame_has_mem_mapped_io(physical_address frame_base_addr) {
    if (frame_base_dne(frame_base_addr)) {
        return (mem_mapped_io_resp) {
            status: MEM_MAPPED_IO_FRAME_BASE_DNE,
            has_mem_mapped_io: false
        };
    }

    addr_range frame_addr_range = new_addr_range(frame_base_addr, BYTES_PER_MEMORY_BLOCK);

    for (unsigned int i = 0; i < next_io_range_index; i++) {
        addr_range curr_io_range = all_potential_io_ranges[i];
        if (ranges_overlap(frame_addr_range, curr_io_range)) {
            return (mem_mapped_io_resp) {
                status: MEM_MAPPED_IO_SUCCESS,
                has_mem_mapped_io: true
            };
        }
    }

    return (mem_mapped_io_resp) {
        status: MEM_MAPPED_IO_SUCCESS,
        has_mem_mapped_io: false
    };
}

typedef enum save_potential_io_range_status {
    SAVE_POTENTIAL_IO_RANGE_SUCCESS = 0,
    SAVE_POTENTIAL_IO_RANGE_FAILED_TO_ADD_TO_BUFFER = 1
} save_potential_io_range_status;

save_potential_io_range_status save_potential_io_range(physical_address potential_io_region_base_addr,
                                                       unsigned int potential_io_region_length) {

    if (next_io_range_index == MAX_NUM_POTENTIAL_IO_RANGES) {
        return SAVE_POTENTIAL_IO_RANGE_FAILED_TO_ADD_TO_BUFFER;
    }

    physical_address last_addr_in_range = potential_io_region_base_addr + potential_io_region_length - 1;

    all_potential_io_ranges[next_io_range_index] = \
        (addr_range) {
            base_addr: potential_io_region_base_addr,
            end_addr_incl: last_addr_in_range
        };

    next_io_range_index += 1;
    return SAVE_POTENTIAL_IO_RANGE_SUCCESS;
}

void classify_mem_region(mem_map_entry* mem_region_entry) {
    unsigned int entry_base_addr = mem_region_entry -> base_addr_low;
    unsigned int region_length = mem_region_entry -> length_in_bytes_low;

    if (mem_region_entry -> type == AVAILABLE_TO_OS) {
        free_mem_blocks_of_addr_range(entry_base_addr, region_length);
    } else if (mem_region_entry -> type == SYSTEM_RESERVED) {
        save_potential_io_range((physical_address) entry_base_addr, region_length);
        set_mem_blocks_of_address_range(entry_base_addr, region_length);
    } else {
        set_mem_blocks_of_address_range(entry_base_addr, region_length);
    }

}

void config_mem_regions(boot_info* curr_boot_info) {

    unsigned int num_mem_map_entries = curr_boot_info -> mem_map_num_entries;
    mem_map_entry* mem_map_entry_list_base_addr = curr_boot_info -> mem_map_entry_list_base_addr;

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

bool config_finished = false;

bool phys_mem_needs_config() {
    return !config_finished;
}

#define BYTES_PER_KB 1024

void config_phys_mem(boot_info* curr_boot_info) {
	unsigned int num_kb_in_mem = curr_boot_info -> num_kb_in_mem;
    unsigned int kb_per_mem_block = BYTES_PER_MEMORY_BLOCK / BYTES_PER_KB;
	num_accessible_memory_blocks = num_kb_in_mem / kb_per_mem_block;

    set_all_mem_blocks();
	config_mem_regions(curr_boot_info);

    config_finished = true;
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
    GET_FIRST_FREE_BLOCK_ALL_IN_USE = 1,
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
        return (get_first_free_block_num_resp) {
            status: GET_FIRST_FREE_BLOCK_ALL_IN_USE,
            block_num: 0};
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
    GET_NEXT_FREE_FRAME_ALL_IN_USE = 1,
    GET_NEXT_FREE_FRAME_FAILED_GET_FIRST_FREE_BLOCK = 2
} get_next_free_frame_status;

typedef struct get_next_free_frame_resp {
    get_next_free_frame_status status;
    physical_address next_free_frame;
} get_next_free_frame_resp;

get_next_free_frame_resp get_next_free_frame() {
    get_first_free_block_num_resp free_block_num_resp = get_first_free_block_num();
    if (free_block_num_resp.status == GET_FIRST_FREE_BLOCK_NUM_SUCCESS) {
    } else if (free_block_num_resp.status == GET_FIRST_FREE_BLOCK_ALL_IN_USE) {
        return (get_next_free_frame_resp) {
            status: GET_NEXT_FREE_FRAME_ALL_IN_USE,
            next_free_frame: 0};
    } else {
        return (get_next_free_frame_resp) {
            status: GET_NEXT_FREE_FRAME_FAILED_GET_FIRST_FREE_BLOCK,
            next_free_frame: 0};
    }

    return (get_next_free_frame_resp) {
        status: GET_NEXT_FREE_FRAME_SUCCESS,
        next_free_frame: BYTES_PER_MEMORY_BLOCK * free_block_num_resp.block_num};
}

bool frame_is_in_use(physical_address frame_base) {
    unsigned int block_num = frame_base / BYTES_PER_MEMORY_BLOCK;
    return block_is_set(block_num);
}

alloc_spec_frame_resp alloc_spec_frame(physical_address frame_base_addr) {
    if (frame_base_dne(frame_base_addr)) {
        return (alloc_spec_frame_resp) {
            status: ALLOC_SPEC_FRAME_BASE_DNE,
            buffer_size: 0,
            buffer_base_addr: 0};
    }

    if (frame_is_in_use(frame_base_addr)) {
        return (alloc_spec_frame_resp) {
            status: ALLOC_SPEC_FRAME_IN_USE,
            buffer_size: 0,
            buffer_base_addr: 0};
    }

    unsigned int block_num = frame_base_addr / BYTES_PER_MEMORY_BLOCK;
	set_memory_block(block_num);

    return (alloc_spec_frame_resp) {
        status: ALLOC_SPEC_FRAME_SUCCESS,
        buffer_size: BYTES_PER_MEMORY_BLOCK,
        buffer_base_addr: frame_base_addr};
}

alloc_block_resp alloc_block() {
    get_next_free_frame_resp next_free_frame_resp = get_next_free_frame();
    if (next_free_frame_resp.status == GET_NEXT_FREE_FRAME_SUCCESS) {
    } else if (next_free_frame_resp.status == GET_NEXT_FREE_FRAME_ALL_IN_USE) {
        return (alloc_block_resp) {
            status: ALLOC_BLOCK_ALL_BLOCKS_IN_USE,
            buffer_size: 0,
            buffer_base_addr: 0};
    } else {
        return (alloc_block_resp) {
            status: ALLOC_BLOCK_FAILED_GET_FREE_FRAME,
            buffer_size: 0,
            buffer_base_addr: 0};
    }

    alloc_spec_frame_resp spec_frame_resp = alloc_spec_frame(next_free_frame_resp.next_free_frame);
    if (spec_frame_resp.status == ALLOC_SPEC_FRAME_SUCCESS) {
    } else {
        return (alloc_block_resp) {
            status: ALLOC_BLOCK_FAILED_ALLOC_SPEC_FRAME,
            buffer_size: 0,
            buffer_base_addr: 0};
    }

    return (alloc_block_resp) {status: ALLOC_BLOCK_SUCCESS,
                               buffer_base_addr: spec_frame_resp.buffer_base_addr,
                               buffer_size: BYTES_PER_MEMORY_BLOCK};
}

void free_block(physical_address block_address) {
    free_mem_blocks_of_addr_range(block_address, BYTES_PER_MEMORY_BLOCK);
}

void clear_phys_mem_config() {
    set_all_mem_blocks();
    config_finished = false;
}
