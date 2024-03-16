#include "timer.h"

#define MS_TO_TICKS(s) ((TIMER_FREQ * s) / 1000) 
#define TICKS_TO_MS(t) (t * (1000 / TIMER_FREQ))

uint64_t volatile ticks;

void timer_handler(Registers* regs) { 
    ticks ++; 
}

void timer_pit_init(uint32_t divisor) { 
    port_byte_out(MODE_COMMAND_PORT, SELECT_CHANNEL_0 | ACCESS_MODE_LO_HI_BYTE | MODE_3);
    set_pit_channel_count(CHANNEL0_PORT, divisor);
}

void timer_initialize() { 
    ticks = 0;
    set_IRQHandler(TIMER_IRQ, timer_handler);
    uint32_t divisor = PIT_FREQ/TIMER_FREQ;
    timer_pit_init(divisor);
}

void sleep(uint32_t milliseconds) { 
    uint64_t target_ticks = ticks + MS_TO_TICKS(milliseconds);
    while (ticks < target_ticks) { 
        __asm__ volatile ("hlt"); 
    }
}

int time_now() { 
    return TICKS_TO_MS(ticks);
}


