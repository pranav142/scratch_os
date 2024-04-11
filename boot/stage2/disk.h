#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "x86.h"

typedef struct {
  uint8_t id;
  uint8_t drive_type;
  uint16_t cylinders;
  uint16_t sectors;
  uint16_t heads;
} DISK;

typedef struct { 
  uint16_t sizeResultBuffer;
  uint16_t infoFlags;
  uint32_t numCylinders;
  uint32_t numHeads;
  uint32_t numSectorsPerTrack;
  uint64_t numSectors;
  uint16_t bytesPerSector;
  uint32_t optional;
} __attribute__((packed)) Disk_Params;

void linear_to_segment_offset(uint32_t linear_address, uint16_t* segment_out, uint16_t* offset_out);
void lba_to_chs(uint32_t lba, uint8_t* cylinder_out, uint8_t* head_out, uint8_t* sector_out,  uint8_t heads_per_cylinder, uint8_t sectors_per_track);

bool disk_read(uint8_t drive, uint32_t lba, uint16_t num_sectors, uint32_t storage_address);
bool get_advanced_disk_params(uint8_t drive, Disk_Params* disk_params);