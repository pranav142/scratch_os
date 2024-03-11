#include "isr.h"
#include "idt.h"
#include "../drivers/screen.h"

void __attribute((cdecl)) ISR1(); 

void __attribute__((cdecl)) ISR_Handler(Registers* regs) { 
    printf("Interrupt %d", regs->interrupt);
}

void ISR_initialize() {
    IDT_SetGate(1, ISR1, CODE_SEGMENT, IDT_FLAG_GATE_32_INT | IDT_FLAG_RING_0);
    IDT_enableGate(1);  
}






