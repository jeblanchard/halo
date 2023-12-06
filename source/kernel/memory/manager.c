#include "kernel/memory/physical/physical_mem.h"
#include "manager.h"
#include "stddef.h"
#include "stdio.h"

halloc_resp halloc(unsigned int num_bytes) {
    (void) num_bytes;

    alloc_block_resp alloc_block_resp = alloc_block();

    if (alloc_block_resp.status == ALLOC_BLOCK_SUCCESS) {
        return (halloc_resp) {status: HALLOC_SUCCESS,
                              buffer: (void*) alloc_block_resp.buffer};
    }

    return (halloc_resp) {status: NOT_ENOUGH_MEM,
                          buffer: NULL};
}
