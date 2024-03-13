#pragma once

#include <stdint.h>
#include <stddef.h>

void ISR_initialize();

typedef struct 
{
    // reverse order interrupt pushes to stack
    uint32_t ds;                                            
    uint32_t edi, esi, ebp, esp_2, ebx, edx, ecx, eax;    
    uint32_t interrupt, error;                              
    uint32_t eip, cs, eflags, esp, ss;                  
} __attribute__((packed)) Registers;

typedef void (*ISRHandler)(Registers* regs);

void set_ISRHandler(int interrupt, ISRHandler handler);
