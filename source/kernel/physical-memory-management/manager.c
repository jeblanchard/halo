#include "../utils/errors.h"
#include "../utils/standard.h"
#include "../utils/memory.h"
#include "../boot.h"
#include "../drivers/screen.h"

// Block alignment
#define BLOCK_ALIGNMENT	BYTES_PER_MEMORY_BLOCK

// 4 GB
#define MAX_MEMORY_32_BIT_IN_BYTES 0x40000000

#define BYTES_PER_MEMORY_BLOCK 4096

#define NUM_MEMORY_BLOCKS MAX_MEMORY_32_BIT_IN_BYTES / BYTES_PER_MEMORY_BLOCK

#define BITS_PER_MEMORY_MAP_SECTION 32

#define NUM_MEMORY_MAP_SECTIONS NUM_MEMORY_BLOCKS / BITS_PER_MEMORY_MAP_SECTION

unsigned int memory_map[NUM_MEMORY_MAP_SECTIONS];

#define BYTES_PER_KB 1024

// Number of memory blocks
static unsigned int num_accessible_memory_blocks;

unsigned int get_memory_map_section(unsigned int block_num) {
    return block_num / BITS_PER_MEMORY_MAP_SECTION;
}

unsigned int get_memory_map_section_offset_mask(unsigned int block_num) {
    unsigned int block_mask = 1 << (block_num % BITS_PER_MEMORY_MAP_SECTION);
    return block_mask;
}

void clear_memory_block(unsigned int block_num) {
    unsigned int block_section = get_memory_map_section(block_num);
    unsigned int block_mask = get_memory_map_section_offset_mask(block_num);

    memory_map[block_section] &= ~ block_mask;
}

// Number of blocks currently in use
static unsigned int num_blocks_in_use;

typedef unsigned int physical_address;

void free_memory_blocks_of_address_range(physical_address base_addr,
                                         unsigned int range_length) {

    unsigned int end_addr = base_addr + range_length;

	unsigned int start_block_num = base_addr / BYTES_PER_MEMORY_BLOCK;
	unsigned int end_block_num = end_addr / BYTES_PER_MEMORY_BLOCK + 1;

    for (unsigned int b = start_block_num; b <= end_block_num; b++) {
        clear_memory_block(b);
        num_blocks_in_use--;
    }
}

void set_memory_block(unsigned int block_num) {
    unsigned int block_section = get_memory_map_section(block_num);
    unsigned int block_mask = get_memory_map_section_offset_mask(block_num);

    memory_map[block_section] |= block_mask;
}

void set_memory_blocks_of_address_range(physical_address base_addr,
                                        unsigned int range_length) {

    unsigned int end_addr = base_addr + range_length;

	unsigned int start_block_num = base_addr / BYTES_PER_MEMORY_BLOCK;
	unsigned int end_block_num = end_addr / BYTES_PER_MEMORY_BLOCK + 1;

    for (unsigned int b = start_block_num; b <= end_block_num; b++) {
        set_memory_block(b);
        num_blocks_in_use++;
    }
}

void allocate_memory_for_memory_map_entry(struct memory_map_entry* entry) {
    unsigned int entry_base_addr = entry -> base_addr_low;
    unsigned int region_length = entry -> length_low;

    memory_range_type entry_type = entry -> type;

    if (entry_type == AVAILABLE_RAM) {
        free_memory_blocks_of_address_range(entry_base_addr, region_length);
    } else {
        set_memory_blocks_of_address_range(entry_base_addr, region_length);
    }

}

void reserve_bios_memory(struct multiboot2_info* boot_info) {

    unsigned int num_mem_map_entries = boot_info -> mem_map_num_entries;

    struct memory_map_entry* mem_map_entry_list_base_addr = boot_info -> mem_map_entry_list_base_addr;

    for (unsigned int i = 0; i < num_mem_map_entries; i++) {
        struct memory_map_entry* entry = mem_map_entry_list_base_addr + i;

        allocate_memory_for_memory_map_entry(entry);
    }
}

void set_all_memory_blocks() {
    void* memory_map_base_address = &memory_map[0];
    unsigned int num_bytes_to_null_out = sizeof(memory_map);
    set_memory(memory_map_base_address, 0xff, num_bytes_to_null_out);
}

void initialize_physical_mem_manager(struct multiboot2_info* boot_info) {

	unsigned int num_kb_in_memory = boot_info -> num_kb_in_mem;
	num_accessible_memory_blocks = (num_kb_in_memory * BYTES_PER_KB) / BYTES_PER_MEMORY_BLOCK;

    // To start, all of memory is in use
    num_blocks_in_use = num_accessible_memory_blocks;
    set_all_memory_blocks();

	reserve_bios_memory(boot_info);
}

unsigned int get_num_memory_map_sections() {
    return num_accessible_memory_blocks / BITS_PER_MEMORY_MAP_SECTION;
}

bool block_is_set(unsigned int block_num) {
    unsigned int block_section = get_memory_map_section(block_num);
    unsigned int block_mask = get_memory_map_section_offset_mask(block_num);

    unsigned int result = memory_map[block_section] & block_mask;

    if (result != 0) {
        return true;
    }

    return false;
}

// 8 blocks per byte
#define BLOCKS_PER_BYTE 8

#define MEMORY_MAP_SECTION_MAX_VALUE 0xffffffff

bool memory_map_section_has_space(unsigned int section_num) {
    if (memory_map[section_num] != MEMORY_MAP_SECTION_MAX_VALUE) {
        return true;
    }

    return false;
}

bool offset_in_section_is_free(unsigned int section_num, unsigned int offset) {
    unsigned int block_mask = 1 << offset;

    if ((memory_map[section_num] & block_mask) != 0) {
        return false;
    }

    return true;
}

unsigned int get_offset_of_first_free_block_in_memory_map_section(unsigned int section_num) {

    unsigned int o = 0;
    bool could_not_find_offset = true;
    for (; o < BITS_PER_MEMORY_MAP_SECTION; o++) {
        if (offset_in_section_is_free(section_num, o)) {
            could_not_find_offset = false;
            break;
        }
    }

    if (could_not_find_offset) {
        char error_msg[] = "Could not find offset of first free block in\nmemory map section.";
        halt_and_display_error_msg(error_msg);
    }

    return o;
}

unsigned int get_first_free_block_num() {

    unsigned int section_num = 0;
    unsigned int section_offset = 0;

    bool block_found = false;
	for (unsigned int s = 0; s < get_num_memory_map_sections(); s++) {
		if (memory_map_section_has_space(s)) {
		    section_offset = get_offset_of_first_free_block_in_memory_map_section(s);
		    section_num = s;

            block_found = true;
		    break;
		}
	}

	if (!block_found) {
	    char error_msg[] = "Could not find free memory block.";
        halt_and_display_error_msg(error_msg);
	}

    unsigned int block_num = section_num * BITS_PER_MEMORY_MAP_SECTION + section_offset;
	return block_num;
}

unsigned int get_num_free_blocks() {
    return num_accessible_memory_blocks - num_blocks_in_use;
}

void* allocate_block() {

	if (get_num_free_blocks() <= 0) {
	    char error_msg[] = "No free memory blocks.";
	    halt_and_display_error_msg(error_msg);
	}

	int block_num = get_first_free_block_num();

	set_memory_block(block_num);

	physical_address block_address = block_num * BYTES_PER_MEMORY_BLOCK;

	num_blocks_in_use++;

	return (void*) block_address;
}
