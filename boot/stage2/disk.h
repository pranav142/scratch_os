#pragma once

#include <stdint.h>

typedef struct {
  uint8_t id;
  uint8_t drive_type;
  uint16_t cylinders;
  uint16_t sectors;
  uint16_t heads;
} DISK;
