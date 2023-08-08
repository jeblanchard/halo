COMMAND_REG                    equ 0x300

PAGE_START                 equ COMMAND_REG + 1

PAGE_STOP                  equ COMMAND_REG + 2

BOUNDARY_REG                   equ COMMAND_REG + 3

TRANSMIT_STATUS            equ COMMAND_REG + 4
TRANSMIT_PAGE              equ COMMAND_REG + 4

TRANSMIT_BYTE_COUNT_0      equ COMMAND_REG + 5
NCR                        equ COMMAND_REG + 5

TRANSMIT_BYTE_COUNT_1      equ COMMAND_REG + 6

INTERRUPT_STATUS           equ COMMAND_REG + 7
CURRENT                    equ COMMAND_REG + 7         ; in page 1

REMOTE_START_ADDRESS_0     equ COMMAND_REG + 8
CR_DMA_0                   equ COMMAND_REG + 8

REMOTE_START_ADDRESS_1     equ COMMAND_REG + 9
CR_DMA_1                   equ COMMAND_REG + 9

REMOTE_BYTE_COUNT_0        equ COMMAND_REG + 0x0a
REMOTE_BYTE_COUNT_1        equ COMMAND_REG + 0x0b

RECEIVE_STATUS             equ COMMAND_REG + 0x0c
RECEIVE_CONFIG_REG      equ COMMAND_REG + 0x0c

TRANSMIT_CONFIG_REG     equ COMMAND_REG + 0x0d
FAE_TALLY                  equ COMMAND_REG + 0x0d

DATA_CONFIG_REG            equ COMMAND_REG + 0x0e
CRC_TALLY                  equ COMMAND_REG + 0x0e

INTERRUPT_MASK_REG             equ COMMAND_REG + 0x0f
MISS_PKT_TALLY             equ COMMAND_REG + 0x0f

IO_PORT                    equ COMMAND_REG + 0x10

P_START equ 0x46
P_STOP equ 0x80

TRANSMIT_BUFFER equ 0x40
