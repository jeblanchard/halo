#include "../utils/standard.h"

static unsigned char received_packet_buffer[PACKET_BUFFER_SIZE_IN_BYTES]

global void handle_received_packet() {
    // do nothing right now
}

struct data_byte {
    unsigned char packet_id;

    unsigned char data_byte;
}

#define MAX_NUM_BYTES_OF_DATA_IN_BUFFER 128

// A buffer for packets-to-be-transmitted. Wraps around once a packet
// reaches the last index.
static struct data_wrapper transmission_packet_queue_ring[PACKET_BUFFER_SIZE_IN_BYTES]

static unsigned short num_packets_in_queue;

bool transmission_packet_queue_is_empty() {
    if (num_packets_in_queue == 0) {
        return true;
    }

    return false;
}

global void check_queue() {
    if (transmission_packet_queue_is_empty) {
        // do nothing for now
    }
}

unsigned short next_packet_start_index;

unsigned short next_queue_index;

bool queue_is_full() {
    if (next_queue_index == next_packet_start_index) {
        return true;
    }

    return false;
}

void add_byte_to_queue(unsigned char* data_byte_address) {
    // check if the space is already taken
    // if it is, the queue is full and we have
    // to rewind everything.
    if (queue_is_full()) {
        char msg[] = "Transmission queue is full.";
        halt_and_display_error_msg()
    }

    transmission_packet_queue_ring[next_queue_index] = *data_byte_address;
    next_queue_index++;
}

void load_packet_to_queue_buffer(unsigned int pointer_to_packet,
                                 unsigned short byte_count) {

    unsigned short num_bytes_loaded_into_queue = 0;
    for (int i = 0; i < byte_count; i++) {
        if (i == PACKET_BUFFER_SIZE_IN_BYTES) {
            next_queue_index = 0;
        }

        transmission_packet_queue_ring[next_queue_index].part_of_packet = true;

        unsigned char next_data_byte = [pointer_to_packet + i];
        transmission_packet_queue_ring[next_queue_index].data_byte = next_data_byte;
    }
}

unsigned void* get_address_of_next_packet() {
//    return *transmission_packet_queue_ring[next_packet_start];

    if (transmission_packet_queue_is_empty()) {
        char msg[] = "There is no packet in the transmission queue.";
        halt_and_display_error_msg(msg);
    }

    next_packet_start_data

    if (transmission_packet_queue_is_empty()) {
        char msg[] = "There is no packet in the transmission queue.";
        halt_and_display_error_msg(msg);
    }
}

unsigned void* get_byte_count_of_next_packet() {
    unsigned short byte_count;

    if (next_packet_end < next_packet_start) {

    }
}