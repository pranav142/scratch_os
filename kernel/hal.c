#include "hal.h"

void HAL_initialize() { 
    IDT_initialize();
    ISR_initialize(); 
    IRQ_initialize();
    IRQ_set_mask(0);
    keyboard_initialize();
}
