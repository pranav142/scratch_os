#include "hal.h"

void HAL_initialize() { 
    IDT_initialize();
    ISR_initialize(); 
    IRQ_initialize();
    timer_initialize();
    keyboard_initialize();
}
