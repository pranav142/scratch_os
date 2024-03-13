#pragma once

#include "../drivers/pic.h"
#include "../drivers/ports.h"
#include "../drivers/screen.h"
#include "isr.h"

#define PIC1_OFFSET 0x20
#define PIC2_OFFSET PIC1_OFFSET + 8

void IRQ_initialize();
void set_IRQHandler(int IRQline, ISRHandler handler);

