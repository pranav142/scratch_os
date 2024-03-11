#include "isr.h"
#include "idt.h"
#include "../drivers/screen.h"

void __attribute__((cdecl)) ISR0(); 

void __attribute__((cdecl)) ISR_Handler(Registers* regs) { 
    printf("Interrupt");
}

void ISR_initialize() {
    IDT_SetGate(0, ISR0, CODE_SEGMENT, IDT_FLAG_GATE_32_TRAP | IDT_FLAG_RING_0);
    IDT_enableGate(0);  
}






