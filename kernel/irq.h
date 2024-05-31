#pragma once

#include "../drivers/pic.h"
#include "../drivers/ports.h"
#include "../drivers/screen.h"
#include "isr.h"

#define PIC1_OFFSET 0x20
#define PIC2_OFFSET PIC1_OFFSET + 8
#define TOTAL_HARDWARE_INTERRUPTS 16

void set_IRQ_handler(int IRQ_line, ISRHandler handler);
void IRQ_common_handler(Registers *regs);
void IRQ_initialize();
