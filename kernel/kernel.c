#include "../drivers/screen.h"
#include "../drivers/ports.h"
#include "idt.h"
#include "isr.h"

void main() {
    clear_screen();
    printf("Welcome to Scratch OS \n");

    IDT_initialize();
    ISR_initialize(); 
    printf("Interrupts Initialized\n");

    printf("test exception interrupt 0x0 division by 0\n");
    __asm__("int $0x0");
}


