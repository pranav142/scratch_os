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

typedef struct { 
  uint8_t fileName[11];
  uint8_t fileAttributes;
  uint8_t reserved;
  uint8_t creationTimeMs;
  uint16_t creationTime;
  uint16_t creationDate;
  uint16_t lastAccessedDate;
  uint16_t clusterHiBytes;
  uint16_t lastModificationTime;
  uint16_t lastModifcationDate;
  uint16_t clusterLoBytes;
  uint32_t fileSize;
} __attribute__((packed)) Directory_Entry;

typedef enum {
  READ_ONLY = 0x1,
  HIDDEN = 0x02,
  SYSTEM = 0x04,
  VOLUME_ID = 0x08,
  DIRECTORY = 0x10,
  ARCHIVE = 0x20,
} FileAttributes;

#define DIRECTORY_ENTRY_BYTE_SIZE 32
void printFAT12BPB(const FAT12_BPB *bpb);

