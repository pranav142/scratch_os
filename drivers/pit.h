#pragma once

#include "ports.h"
#include <stdint.h>

#define CHANNEL0_PORT 0x40
#define CHANNEL1_PORT 0x41
#define CHANNEL2_PORT 0x42
#define MODE_COMMAND_PORT 0x43

#define PIT_FREQ 1193180

typedef enum {
  SELECT_CHANNEL_0 = 0x0 << 6,
  SELECT_CHANNEL_1 = 0x1 << 6,
  SELECT_CHANNEL_2 = 0x2 << 6,
  READ_BACK_COMMAND = 0x3 << 6,

  LATCH_COUNT = 0x0 << 4,
  ACCESS_MODE_LO_BYTE = 0x1 << 4,
  ACCESS_MODE_HI_BYTE = 0x2 << 4,
  ACCESS_MODE_LO_HI_BYTE = 0x3 << 4,

  MODE_0 = 0x0 << 1,
  MODE_1 = 0x1 << 1,
  MODE_2 = 0x2 << 1,
  MODE_3 = 0x3 << 1,
  MODE_4 = 0x4 << 1,
  MODE_5 = 0x5 << 1,

  BCD_MODE = 0x1,
} PIT_FLAGS;

void set_pit_channel_count(uint8_t channel, uint32_t count);
uint16_t read_pit_count();
