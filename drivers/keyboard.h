#pragma once

#include "../kernel/irq.h"
#include "ports.h"

#define KEYBOARD_IRQLINE 1


void keyboard_initialize();
void keyboard_handler(Registers* regs);  

