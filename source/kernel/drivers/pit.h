#define PIT_IRQ_NUM 0

#define		PIT_COUNTER_0_REG		    0x40
#define		PIT_COUNTER_1_REG		    0x41
#define		PIT_COUNTER_2_REG		    0x42
#define		PIT_COMMAND_REG		        0x43

void initialize_pit();

// Handles the PIT IRQ
void pit_irq_handler();

// Starts a PIT counter.
//
// freq:
//   The frequency of a timer tick in Hz.
//
// ocw:
//
// A set of flags detailing the behavior
// of the PIT.
//
// Bit 0: (BCP) Binary Counter
//   0: Binary
//   1: Binary Coded Decimal (BCD)
// Bit 1-3: (M0, M1, M2) Operating Mode
//   000: Mode 0: Interrupt or Terminal Count
//   001: Mode 1: Programmable one-shot
//   010: Mode 2: Rate Generator
//   011: Mode 3: Square Wave Generator
//   100: Mode 4: Software Triggered Strobe
//   101: Mode 5: Hardware Triggered Strobe
//   110: Undefined; Don't use
//   111: Undefined; Don't use
// Bits 4-5: (RL0, RL1) Read/Load Mode
//   00: Counter value is latched into an internal control
//       register at the time of the I/O write operation.
//   01: Read or Load Least Significant Byte (LSB) only
//   10: Read or Load Most Significant Byte (MSB) only
//   11: Read or Load LSB first then MSB
// Bits 6-7: (SC0-SC1) Select Counter
//   00: Counter 0
//   01: Counter 1
//   10: Counter 2
//   11: Illegal value
void pit_set_counter(unsigned int freq, unsigned char ocw);
