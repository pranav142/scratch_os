#pragma once
#include <stdint.h> 

typedef enum { 
    IDT_FLAG_GATE_TASK      = 0x5,
    IDT_FLAG_GATE_16_INT    = 0x6,
    IDT_FLAG_GATE_16_TRAP   = 0x7,
    IDT_FLAG_GATE_32_INT    = 0xE,
    IDT_FLAG_GATE_32_TRAP   = 0xF,

    IDT_FLAG_RING_0         = (0 << 5),
    IDT_FLAG_RING_1         = (1 << 5),
    IDT_FLAG_RING_2         = (2 << 5),
    IDT_FLAG_RING_3         = (3 << 5),

    IDT_FLAG_PRESENT        = (1 << 7),
} IDT_FLAGS;

void IDT_initialize();
void IDT_disableGate(int interrupt);
void IDT_enableGate(int interrupt);
void IDT_SetGate(int interrupt, void * ISR_Addr, uint8_t segment_selector, uint8_t flags);