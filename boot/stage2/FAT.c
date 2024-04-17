#include "FAT.h"

void printFAT12BPB(const FAT12_BPB *bpb) {
  printf("Bytes per Sector: %u\n", bpb->bytesPerSector);
  printf("Sectors per Cluster: %u\n", bpb->sectorsPerCluster);
  printf("Reserved Sectors: %u\n", bpb->reservedSectors);
  printf("Number of FATs: %u\n", bpb->numFATs);
  printf("Max Root Directory Entries: %u\n", bpb->maxRootDirEntries);
  printf("Total Sectors: %u\n", bpb->totalSectors);
  printf("Media Descriptor: %X\n", bpb->mediaDescriptor);
  printf("Sectors per FAT: %u\n", bpb->sectorsPerFAT);
  printf("Sectors per Track: %u\n", bpb->sectorsPerTrack);
  printf("Number of Heads: %u\n", bpb->numHeads);
  printf("Hidden Sectors: %u\n", bpb->hiddenSectors);
  printf("Large Total Sectors: %u\n", bpb->largeTotalSectors);
  printf("Physical Drive Number: %u\n", bpb->physicalDriveNumber);
  printf("Reserved: %u\n", bpb->reserved);
  printf("Extended Boot Signature: %X\n", bpb->extendedBootSignature);
  printf("Volume Serial Number: %u\n", bpb->volumeSerialNumber);
  printf("Volume Label: %s\n", bpb->volumeLabel);
  printf("Filesystem Type: %s\n", bpb->filesystemType);
}

uint32_t get_next_cluster(uint8_t *FAT, uint32_t current_cluster) {
  uint32_t byte_offset = (current_cluster * 3) / 2;
  uint32_t next_cluster;

  if (current_cluster & 1) {
    next_cluster = (FAT[byte_offset] >> 4) | (FAT[byte_offset + 1] << 4);
  } else {
    next_cluster = FAT[byte_offset] | ((FAT[byte_offset + 1] & 0x0F) << 8);
  }

  return next_cluster & 0x0FFF;
}

bool read_directory_buffer(FAT12_BPB *fat12_bpb, DISK *disk,
                           Directory_Entry *directory_buffer) {
  int error;
  int directory_sector = fat12_bpb->reservedSectors +
                         fat12_bpb->numFATs * fat12_bpb->sectorsPerFAT;
  int directory_num_sectors =
      (fat12_bpb->maxRootDirEntries * DIRECTORY_ENTRY_BYTE_SIZE) /
      fat12_bpb->bytesPerSector;

  int size = directory_num_sectors * 512;
  error = disk_read(disk->id, directory_sector, directory_num_sectors,
                    (uintptr_t)directory_buffer, disk->headsPerCylinder,
                    disk->sectorsPerTrack);
  return error;
}

uint32_t get_starting_cluster(Directory_Entry *entry) {
  return (entry->clusterHiBytes << 16) | entry->clusterLoBytes;
}
void print_directory_entry(Directory_Entry *entry) {
  uint32_t starting_cluster = get_starting_cluster(entry);

  printf("File Name: ");
  for (int i = 0; i < 11; i++) {
    printf("%c", entry->fileName[i]);
  }
  printf("\n");

  printf("File Attributes: %X\n", entry->fileAttributes);
  printf("Reserved: %X\n", entry->reserved);
  printf("Creation Time MS: %u\n", entry->creationTimeMs);
  printf("Creation Time: %u\n", entry->creationTime);
  printf("Creation Date: %u\n", entry->creationDate);
  printf("Last Accessed Date: %u\n", entry->lastAccessedDate);
  printf("Cluster High Bytes: %u\n", entry->clusterHiBytes);
  printf("Last Modification Time: %u\n", entry->lastModificationTime);
  printf("Last Modification Date: %u\n", entry->lastModifcationDate);
  printf("Cluster Low Bytes: %u\n", entry->clusterLoBytes);
  printf("File Size: %u bytes\n", entry->fileSize);
  printf("Starting Cluster: %u\n", starting_cluster);
}

Directory_Entry *find_file(const char *filename,
                           Directory_Entry *directory_buffer,
                           uint32_t directory_entry_count) {
  const uint8_t FILE_SIZE = 11;
  for (int i = 0; i < directory_entry_count; i++) {
    if (memcmp(filename, directory_buffer[i].fileName, FILE_SIZE) == 0)
      return &directory_buffer[i];
  }

  return 0;
}

bool read_FAT(uint8_t *fat_buffer, DISK *disk, FAT12_BPB *fat12_bpb) {
  int error;
  int fat_sector = fat12_bpb->reservedSectors;
  int fat_num_sectors = fat12_bpb->sectorsPerFAT * fat12_bpb->numFATs;
  int fat_size = fat_num_sectors * fat12_bpb->bytesPerSector;

  error =
      disk_read(disk->id, fat_sector, fat_num_sectors, (uintptr_t)fat_buffer,
                disk->headsPerCylinder, disk->sectorsPerTrack);

  return error;
}

bool read_file(Directory_Entry *entry, DISK *disk, FAT12_BPB *fat12_bpb,
               uint8_t *output_buffer, uint8_t *FAT) {
  int error = false;
  uint32_t current_cluster = get_starting_cluster(entry);
  uint32_t directory_sectors =
      (fat12_bpb->maxRootDirEntries * DIRECTORY_ENTRY_BYTE_SIZE) /
      fat12_bpb->bytesPerSector;
  uint32_t fat_sectors = fat12_bpb->numFATs * fat12_bpb->sectorsPerFAT;
  uint32_t directory_end =
      fat12_bpb->reservedSectors + directory_sectors + fat_sectors;

  do {

    uint32_t lba =
        directory_end + (current_cluster - 2) * fat12_bpb->sectorsPerCluster;
    error = disk_read(disk->id, lba, fat12_bpb->sectorsPerCluster,
                      (uintptr_t)output_buffer, disk->headsPerCylinder,
                      disk->sectorsPerTrack);
    output_buffer += fat12_bpb->sectorsPerCluster * fat12_bpb->bytesPerSector;
    current_cluster = get_next_cluster(FAT, current_cluster);
  } while (!error && current_cluster < 0xFF8);

  return error;
}
