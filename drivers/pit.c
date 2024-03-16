#include "pit.h"

void set_pit_channel_count(uint8_t channel, uint32_t count) { 
    disable_interrupts();
    port_byte_out(channel, (uint8_t)(count&0xFF));
    port_byte_out(channel, (uint8_t)((count>>8)&0xFF));
    enable_interrupts();
}

uint16_t read_pit_count() {
  disable_interrupts();
	uint16_t count = 0;
 
	port_byte_out(MODE_COMMAND_PORT, 0b0000000);
 
	count = port_byte_in(CHANNEL0_PORT);		// Low byte
	count |= port_byte_in(CHANNEL0_PORT)<<8;
  
  enable_interrupts();
	return count;
}
