#include "isr.h"
#include "idt.h"
#include "../drivers/screen.h"

void __attribute__((cdecl)) ISR_Handler(Registers* regs) { 
    printf("Interrupt %d", regs->interrupt);
}

void ISR_InitializeGates();

void ISR_initialize() {
    ISR_InitializeGates();
    for(int i=0; i<256; i++) {
        IDT_enableGate(i);
    }
}






