#include "pit.h"

void set_pit_channel_count(uint8_t channel, uint32_t count) { 
    port_byte_out(channel, (uint8_t)(count&0xFF));
    port_byte_out(channel, (uint8_t)((count>>8)&0xFF));
}
