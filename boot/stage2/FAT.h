#pragma once

#include "../../drivers/screen.h"
#include "../../utils/memtools.h"
#include "disk.h"
#include "mem.h"
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
  char fileName[11];
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

#define FAT12_ENTRY_SIZE 12
typedef char FAT_Entry[FAT12_ENTRY_SIZE];

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
uint32_t get_next_cluster(uint8_t *FAT, uint32_t current_cluster);
bool read_directory_buffer(FAT12_BPB *fat12_bpb, DISK *disk,
                           Directory_Entry *directory_buffer);
uint32_t get_starting_cluster(Directory_Entry *entry);
void print_directory_entry(Directory_Entry *entry);
Directory_Entry *find_file(const char *filename,
                           Directory_Entry *directory_buffer,
                           uint32_t directory_entry_count);
bool read_FAT(uint8_t *fat_buffer, DISK *disk, FAT12_BPB *fat12_bpb);
bool read_file(Directory_Entry *entry, DISK *disk, FAT12_BPB *fat12_bpb,
               uint8_t *output_buffer, uint8_t *FAT);
