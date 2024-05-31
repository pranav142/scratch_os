#pragma once

#include "../drivers/ports.h"
#include "../drivers/screen.h"
#include "idt.h"
#include <stddef.h>
#include <stdint.h>

#define TOTAL_TRAP_INTERRUPTS 32

typedef struct {
  // reverse order interrupt pushes to stack
  uint32_t ds;
  uint32_t edi, esi, ebp, esp_2, ebx, edx, ecx, eax;
  uint32_t interrupt, error;
  uint32_t eip, cs, eflags, esp, ss;
} __attribute__((packed)) Registers;

typedef void (*ISRHandler)(Registers *regs);

void set_ISR_handler(int interrupt, ISRHandler handler);
void ISR_initialize();
static void print_handler_debug_info(Registers *regs);
void __attribute__((cdecl)) ISR_common_handler(Registers *regs);
