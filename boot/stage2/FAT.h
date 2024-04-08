#pragma once

#include "../../drivers/screen.h"
#include <stdint.h>

typedef struct {
  uint8_t jump[3];
  char oem[8];
  uint16_t bytesPerSector;
  uint8_t sectorsPerCluster;
  uint16_t reservedSectors;
  uint8_t numFATs;
  uint16_t maxRootDirEntries;
  uint16_t totalSectors;
  uint8_t mediaDescriptor;
  uint16_t sectorsPerFAT;
  uint16_t sectorsPerTrack;
  uint16_t numHeads;
  uint32_t hiddenSectors;
  uint32_t largeTotalSectors;

  uint8_t physicalDriveNumber;
  uint8_t reserved;
  uint8_t extendedBootSignature;
  uint32_t volumeSerialNumber;
  char volumeLabel[11];
  char filesystemType[8];
} __attribute__((packed)) FAT12_BPB;

void printFAT12BPB(const FAT12_BPB *bpb);
