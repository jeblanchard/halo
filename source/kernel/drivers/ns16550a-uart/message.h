#define TX_RX_BUFFER_SIZE 0x7cf

typedef enum {
    SUCCESSFULLY_QUEUED_MSG = 0,
    NO_SPACE_IN_QUEUE = 1,
} queue_msg_for_tx_result;

queue_msg_for_tx_result queue_msg_for_tx(unsigned char * message, unsigned short byte_count);
