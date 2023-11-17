#include "kernel/memory/physical/physical_mem.h"
#include "manager.h"
#include "stddef.h"
#include "stdio.h"

halloc_resp halloc(unsigned int num_bytes) {
    (void) num_bytes;

    block_alloc_resp block_alloc_resp = alloc_block();

    if (block_alloc_resp.status == BLOCK_ALLOC_SUCCESS) {
        return (halloc_resp) {status: HALLOC_SUCCESS,
                              buffer: block_alloc_resp.buffer};
    }

    return (halloc_resp) {status: NOT_ENOUGH_MEM,
                          buffer: NULL};
}
