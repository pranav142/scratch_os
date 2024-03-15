#pragma once

#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "../drivers/keyboard.h"
#include "../drivers/pic.h"
#include "../drivers/timer.h"
void HAL_initialize();
