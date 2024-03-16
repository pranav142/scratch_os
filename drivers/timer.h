#pragma once

#include "../kernel/irq.h"
#include "pit.h"
#include "screen.h"

#define TIMER_FREQ 100
#define TIMER_IRQ 0

void timer_initialize();
void sleep(uint32_t milliseconds);
int time_now();
