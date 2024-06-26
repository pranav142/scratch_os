#pragma once

#include "x86.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  uint8_t id;
  uint8_t drive_type;
  uint16_t cylinders;
  uint16_t sectorsPerTrack;
  uint16_t headsPerCylinder;
} DISK;

#define SUCCESSFUL 0
#define FAILED 1
void linear_to_segment_offset(uintptr_t linear_address, uint16_t *segment_out,
                              uint16_t *offset_out);

void lba_to_chs(uint32_t lba, uint8_t *cylinder_out, uint8_t *head_out,
                uint8_t *sector_out, uint8_t heads_per_cylinder,
                uint8_t sectors_per_track);

bool disk_read(uint8_t drive, uint32_t lba, uint16_t num_sectors,
               uintptr_t storage_address, uint8_t heads_per_cylinder,
               uint8_t sectors_per_track);
