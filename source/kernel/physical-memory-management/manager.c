#include "../utils/errors.h"
#include "../utils/standard.h"
#include "../utils/memory.h"

// Block alignment
#define BLOCK_ALIGNMENT	MEMORY_BLOCK_SIZE

// Pointer to memory map bit array. Each represents a
// memory block
static unsigned int* memory_map;

#define BITS_PER_MEMORY_MAP_SECTION 32

// Number of memory blocks
static unsigned int num_memory_blocks;

unsigned int get_num_memory_map_sections() {
    return num_memory_blocks / BITS_PER_MEMORY_MAP_SECTION;
}

unsigned int get_memory_map_section(unsigned int block_num) {
    return block_num / BITS_PER_MEMORY_MAP_SECTION;
}

unsigned int get_memory_map_section_offset_mask(unsigned int block_num) {
    unsigned int block_mask = 1 << (block_num % BITS_PER_MEMORY_MAP_SECTION);
    return block_mask;
}

void set_memory_block(unsigned int block_num) {
    unsigned int block_section = get_memory_map_section(block_num);
    unsigned int block_mask = get_memory_map_section_offset_mask(block_num);

    memory_map[block_section] |= block_mask;
}

void clear_memory_block(unsigned int block_num) {
    unsigned int block_section = get_memory_map_section(block_num);
    unsigned int block_mask = get_memory_map_section_offset_mask(block_num);

    memory_map[block_section] &= ~ block_mask;
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

typedef unsigned int physical_address;

// 8 blocks per byte
#define BLOCKS_PER_BYTE 8

// Number of blocks currently in use
static unsigned int num_blocks_in_use;

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

// In bytes
#define MEMORY_BLOCK_SIZE 4096

// Size of physical memory
static unsigned int memory_size_;

unsigned int get_num_kb_in_memory() {
    return 0;
    // do nothing for now
}

unsigned int get_block_count() {
    // do nothing rn
    return 0;
}

void initialize_manager(unsigned int memory_size, physical_address bitmap_address) {

	memory_size_ = memory_size;
	memory_map = (unsigned int*) bitmap_address;
	num_memory_blocks =	(get_num_kb_in_memory() * 1024) / MEMORY_BLOCK_SIZE;
	num_blocks_in_use	= num_memory_blocks;

	// To start, all of memory is in use
	unsigned int num_bytes_to_fill = get_block_count() / BLOCKS_PER_BYTE;
	set_memory(memory_map, 0xff, num_bytes_to_fill);
}

unsigned int get_num_free_blocks() {
    return num_memory_blocks - num_blocks_in_use;
}

void* allocate_block() {

	if (get_num_free_blocks() <= 0) {
	    char error_msg[] = "No free memory blocks.";
	    halt_and_display_error_msg(error_msg);
	}

	int block_num = get_first_free_block_num();

	set_memory_block(block_num);

	physical_address block_address = block_num * MEMORY_BLOCK_SIZE;

	num_blocks_in_use++;

	return (void*) block_address;
}

void free_memory_block_of_address(physical_address address) {
	unsigned int block_num = address / MEMORY_BLOCK_SIZE;

	clear_memory_block(block_num);
	num_blocks_in_use--;
}
