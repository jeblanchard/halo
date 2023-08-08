COMMAND                    equ 0x300
PAGE_START                 equ COMMAND + 1
PAGE_STOP                  equ COMMAND + 2
BOUNDARY                   equ COMMAND + 3
TRANSMIT_STATUS            equ COMMAND + 4
TRANSMIT_PAGE              equ COMMAND + 4
TRANSMIT_BYTE_COUNT_0      equ COMMAND + 5
NCR                        equ COMMAND + 5
TRANSMIT_BYTE_COUNT_1      equ COMMAND + 6
INTERRUPT_STATUS           equ COMMAND + 7
CURRENT                    equ COMMAND + 7         ; in page 1
REMOTE_START_ADDRESS_0     equ COMMAND + 8
CR_DMA_0                   equ COMMAND + 8
REMOTE_START_ADDRESS_1     equ COMMAND + 9
CR_DMA_1                   equ COMMAND + 9
REMOTE_BYTE_COUNT_0        equ COMMAND + 0x0a
REMOTE_BYTE_COUNT_1        equ COMMAND + 0x0b
RECEIVE_STATUS             equ COMMAND + 0x0c
RECEIVE_CONFIGURATION      equ COMMAND + 0x0c
TRANSMIT_CONFIGURATION     equ COMMAND + 0x0d
FAE_TALLY                  equ COMMAND + 0x0d
DATA_CONFIGURATION         equ COMMAND + 0x0e
CRC_TALLY                  equ COMMAND + 0x0e
INTERRUPT_MASK             equ COMMAND + 0x0f
MISS_PKT_TALLY             equ COMMAND + 0x0f
IO_PORT                    equ COMMAND + 0x10

P_START equ 0x46
P_STOP equ 0x80

TRANSMIT_BUFFER equ 0x40
