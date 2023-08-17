#include "../utils/standard.h"

#define MAX_NUM_BYTES_IN_RECEIVE_BUFFER 500
static unsigned char received_data_buffer[MAX_NUM_BYTES_IN_RECEIVE_BUFFER];

static unsigned int next_index_to_store_byte_from_nic;

#define MAX_NUM_BYTES_IN_NIC_RECEIVE_BUFFER 500
static unsigned char data_from_nic_buffer[MAX_NUM_BYTES_IN_NIC_RECEIVE_BUFFER];

void advance_next_index_to_store_byte_from_nic() {
    next_index_to_store_byte_from_nic++;
}

static bool packet_is_being_transferred_from_nic;

void start_packet_transfer_from_nic() {
    if (packet_is_being_transferred_from_nic) {
        char err_msg[] = "Packet is already being transferred from NIC.";
        halt_and_display_error_msg(err_msg);
    }

    packet_is_being_transferred_from_nic = true;
    next_index_to_store_byte_from_nic = 0;
}

bool nic_receive_buffer_is_full() {
    if (next_index_to_store_byte_from_nic == MAX_NUM_BYTES_IN_NIC_RECEIVE_BUFFER) {
        return true;
    }

    return false;
}

void transfer_byte_from_nic_to_nic_receive_buffer(unsigned char byte) {
    if (nic_receive_buffer_is_full()) {
        char err_msg[] = "NIC receive buffer is full.";
        halt_and_display_error_msg(err_msg);
    }

    data_from_nic_buffer[next_index_to_store_byte_from_nic] = byte;
    advance_next_index_to_store_byte_from_nic();
}

void transfer_word_from_nic_to_nic_receive_buffer(unsigned short word) {
    unsigned char byte_0 = word;
    unsigned char byte_1 = word >> 8;

    transfer_byte_from_nic_to_nic_receive_buffer(byte_0);
    transfer_byte_from_nic_to_nic_receive_buffer(byte_1);
}

static unsigned int next_index_to_store_byte_in_receive_buffer;

void advance_next_index_to_store_byte_in_receive_buffer() {
    next_index_to_store_byte_in_receive_buffer %= MAX_NUM_BYTES_IN_RECEIVE_BUFFER;
}

void move_data_from_nic_buffer_to_receive_buffer() {
    for (int i = 0; i < next_index_to_store_byte_from_nic; i++) {
        received_data_buffer[next_index_to_store_byte_in_receive_buffer] \
                                               = data_from_nic_buffer[i];
        advance_next_index_to_store_byte_in_receive_buffer();
    }
}

void end_packet_transfer_from_nic() {
    move_data_from_nic_buffer_to_receive_buffer();

    packet_is_being_transferred_from_nic = false;
    next_index_to_store_byte_from_nic = 0;
}

struct transmission_packet {
    bool needs_to_be_transmitted : 1;

    unsigned short data_buffer_start_index;
    unsigned short byte_count;
};

#define MAX_NUM_PACKETS_IN_TRANSMISSION_QUEUE 20
static struct transmission_packet transmission_queue[MAX_NUM_PACKETS_IN_TRANSMISSION_QUEUE]

#define MAX_NUM_BYTES_OF_DATA_IN_BUFFER 1000
static unsigned char transmission_data_buffer[MAX_NUM_BYTES_OF_DATA_IN_BUFFER]

static bool packet_is_being_moved_to_nic;

static unsigned int count_of_bytes_processed;

static struct transmission_packet currently_processing_packet;

// add logic to see if there is even a packet to be
// transferred
void start_packet_transfer_to_nic() {
    packet_is_being_moved_to_nic = true;
    count_of_bytes_processed = 0;

    currently_processing_packet = view_next_packet_to_transmit();
}

unsigned short index_of_next_packet_to_transmit;

struct transmission_packet get_next_packet_to_transmit

bool no_transfer_to_nic_is_happening() {
    if (packet_is_being_transferred_from_nic) {
        return false;
    }

    return true;
}

void advance_index_of_next_packet_to_transmit() {
    index_of_next_packet_to_transmit++;

    if (index_of_next_packet_to_transmit == MAX_NUM_PACKETS_IN_TRANSMISSION_QUEUE) {
        index_of_next_packet_to_transmit = 0;
    }
}

void end_packet_transfer_to_nic() {
    if (no_transfer_to_nic_is_happening()) {
        char bytes_are_left_msg[] = "No packet transfer was initiated.";
        halt_and_display_error_msg(bytes_are_left_msg);
    }

    if (bytes_still_remain_to_be_processed_from_transmitting_packet) {
        char bytes_are_left_msg[] = "Bytes still need to be processed from transmitting packet.";
        halt_and_display_error_msg(bytes_are_left_msg);
    }

    packet_is_being_moved_to_nic = false;
    advance_index_of_next_packet_to_transmit();
}

unsigned int index_of_next_byte_to_transmit;

void advance_index_of_next_byte_to_transmit() {
    index_of_next_byte_to_transmit++;
    index_of_next_byte_to_transmit %= MAX_NUM_BYTES_OF_DATA_IN_BUFFER;
}

bool all_bytes_from_transmitting_packet_have_been_processed() {
    if (count_of_bytes_processed == currently_processing_packet.byte_count) {
        return true;
    }

    return false;
}

bool bytes_still_remain_to_be_processed_from_transmitting_packet() {
    if (count_of_bytes_processed == currently_processing_packet.byte_count) {
        return false;
    }

    return true;
}

unsigned char process_next_transmission_byte() {
    if (no_transfer_to_nic_is_happening()) {
        char no_trans_msg[] = "Transfer to NIC has not been initiated.";
        halt_and_display_error_msg(no_trans_msg);
    }

    if (all_bytes_from_transmitting_packet_have_been_processed()) {
        char no_bytes_msg[] = "No more bytes to process.";
        halt_and_display_error_msg(no_bytes_msg);
    }

    transmission_data_buffer[index_of_next_byte_to_transmit];
    advance_index_of_next_byte_to_transmit();
}

unsigned short process_next_transmission_word() {
    unsigned char byte_0 = process_next_transmission_byte();
    unsigned short byte_1 = process_next_transmission_byte() << 8;

    unsigned short word_to_send = byte_0 | byte_1;
    return word_to_send;
}

static unsigned short num_packets_in_transmission_queue;

bool transmission_queue_is_empty() {
    if (num_packets_in_transmission_queue == 0) {
        return true;
    }

    return false;
}

unsigned int index_of_next_transmission_byte_to_store;

unsigned short in_use_region_start_index;

bool transmission_data_buffer_is_full() {
    if (index_of_next_transmission_byte_to_store == in_use_region_start_index) {
        return true;
    }

    return false;
}

void advance_index_of_next_transmission_byte_to_store() {
    index_of_next_transmission_byte_to_store++;

    if (index_of_next_transmission_byte_to_store == MAX_NUM_BYTES_OF_DATA_IN_BUFFER) {
        index_of_next_transmission_byte_to_store = 0;
    }

    return;
}

void add_byte_to_transmission_data_buffer(unsigned char byte) {
    if (transmission_data_buffer_is_full()) {
        char msg[] = "Transmission data buffer is full.";
        halt_and_display_error_msg()
    }

    packet_data_buffer[index_of_next_transmission_byte_to_store] = byte;
    advance_index_of_next_transmission_byte_to_store();
}

// Returns the start index of the packet's data.
unsigned short add_packet_data_to_transmission_data_buffer(unsigned int packet_address,
                                         unsigned short byte_count) {

    unsigned char* pointer_to_packet = (unsigned char*) packet_address;
    unsigned short og_index_of_next_transmission_byte_to_store = \
                                                   index_of_next_transmission_byte_to_store;

    for (int i = 0; i < byte_count; i++) {
        unsigned char next_data_byte = *(pointer_to_packet + i);
        add_byte_to_transmission_data_buffer(next_data_byte);
    }

    return og_index_of_next_transmission_byte_to_store;
}

static unsigned short index_of_next_packet_to_transmit;

struct transmission_packet view_next_packet_to_transmit() {
    return transmission_queue[index_of_next_packet_to_transmit];
}

unsigned int view_byte_count_of_next_packet_to_transmit() {
    struct transmission_packet next_trans_packet = view_next_packet_to_transmit();
    return next_trans_packet.byte_count;
}

// make sure this isn't used to manually go through data buffer
unsigned char* view_address_of_next_byte_to_transmit() {
    return &packet_data_buffer[index_of_next_transmission_byte_to_store];
}

unsigned short index_of_next_transmission_packet_to_queue;

bool transmission_queue_is_full() {
    struct transmission_packet packet_at_next_slot = transmission_queue[index_of_next_transmission_packet_to_queue];

    if (packet_at_next_slot.needs_to_be_transmitted) {
        return true;
    }

    return false;
}

static unsigned short index_of_next_packet_to_transmit;

void advance_index_of_next_packet_to_transmit() {
    index_of_next_packet_to_transmit++;
    index_of_next_packet_to_transmit %= MAX_NUM_PACKETS_IN_TRANSMISSION_QUEUE;
}

void save_packet_to_transmission_queue(unsigned int byte_count, unsigned short data_start_index) {

    if (transmission_queue_is_full()) {
        char err_msg[] = "Packet storage is full.";
        halt_and_display_error_msg(err_msg);
    }

    transmission_queue[index_of_next_packet_to_transmit].needs_to_be_transmitted = true;
    transmission_queue[index_of_next_packet_to_transmit].data_buffer_start_index = data_start_index;

    advance_index_of_next_packet_to_transmit();
}

void queue_packet_for_transmission(unsigned int packet_address,
                                   unsigned int byte_count) {

    unsigned short data_start_index = add_packet_data_to_transmission_data_buffer(packet_address, byte_count);
    save_packet_to_transmission_queue(byte_count, data_start_index);
}
