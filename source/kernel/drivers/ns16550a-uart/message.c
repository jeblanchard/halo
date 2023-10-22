#include "../../utils/memory.h"
#include <stdbool.h>
#include "message.h"

static unsigned char tx_queue_buffer[TX_RX_BUFFER_SIZE];

static unsigned int next_byte_to_tx_index;
static unsigned int next_free_tx_queue_index;
static unsigned int available_bytes_in_tx_queue_buffer = TX_RX_BUFFER_SIZE;

void set_byte_in_tx_buffer(unsigned int index, unsigned char byte) {
    tx_queue_buffer[index] = byte;
}

bool buffer_is_too_full(unsigned short byte_count) {
    return byte_count > available_bytes_in_tx_queue_buffer;
}

typedef enum {
    SUCESSFULLY_ADVANCED_NEXT_FREE_TX_QUEUE_INDEX = 0,
    COLLIDED_WITH_NEXT_BYTE_TO_TX_INDEX = 1
} advance_next_free_tx_queue_index_result;

advance_next_free_tx_queue_index_result advance_next_free_tx_queue_index() {
    unsigned int new_offset = (next_free_tx_queue_index + 1) % TX_RX_BUFFER_SIZE;

    if (new_offset == next_byte_to_tx_index) {
        return COLLIDED_WITH_NEXT_BYTE_TO_TX_INDEX;
    }

    next_free_tx_queue_index = new_offset;
    return SUCESSFULLY_ADVANCED_NEXT_FREE_TX_QUEUE_INDEX;
}

typedef enum {
    SUCCESSFULLY_QUEUED_BYTE = 0,
    COULD_NOT_ADVANCE_NEXT_FREE_TX_QUEUE_INDEX = 1
} queue_byte_for_tx_result;

queue_byte_for_tx_result queue_byte_for_tx(unsigned char byte) {
    set_byte_in_tx_buffer(next_free_tx_queue_index, byte);

    advance_next_free_tx_queue_index_result result = advance_next_free_tx_queue_index();
    if (result == SUCESSFULLY_ADVANCED_NEXT_FREE_TX_QUEUE_INDEX) {
        return SUCCESSFULLY_QUEUED_BYTE;
    } else {
        return COULD_NOT_ADVANCE_NEXT_FREE_TX_QUEUE_INDEX;
    }
}

void dequeue_last_byte() {
    if (next_free_tx_queue_index == next_byte_to_tx_index) {
        return;
    }

    unsigned int new_offset = (next_free_tx_queue_index - 1) % TX_RX_BUFFER_SIZE;
    next_free_tx_queue_index = new_offset;
}

void restore_queue_space(unsigned int num_bytes) {
    for (unsigned int i = 0; i < num_bytes; i++) {
        dequeue_last_byte();
    }
}

queue_msg_for_tx_result queue_msg_for_tx(unsigned char * message, unsigned short byte_count) {
    if (buffer_is_too_full(byte_count)) {
        return NO_SPACE_IN_QUEUE;
    }

    unsigned short bytes_added = 0;
    for (int i = 0; i < byte_count; i++) {
        queue_byte_for_tx_result res = queue_byte_for_tx(message[i]);
        if (res == SUCCESSFULLY_QUEUED_BYTE) {
            bytes_added += 1;
            continue;
        } else {
            restore_queue_space(bytes_added);
        }
    }

    set_byte_in_tx_buffer(0, 1);

    return SUCCESSFULLY_QUEUED_MSG;
}
