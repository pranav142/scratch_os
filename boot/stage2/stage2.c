#include "stage2.h"
#include "FAT.h"
#include "disk.h"
#include "x86.h"
#include <stdint.h>

#define boot_sect_address 0x7c00
#define DIRECTORY_BUFFER ((Directory_Entry *)0x7e00)
#define FAT_BUFFER ((uint8_t *)0x9b4c)
#define BOOT_SECTOR 0
#define BOOT_SECTOR_LENGTH 1
#define DRIVE_ID 0
#define KERNEL_BUFFER ((uint8_t *)0x30000)
#define KERNEL_LOAD_ADDR (void *)0x100000

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

uint32_t get_cluster_sector(uint16_t cluster, uint8_t sectors_per_cluster,
                            uint32_t first_data_sector) {
  return ((cluster - 2) * sectors_per_cluster) + first_data_sector;
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
  printf("size %d \n", size);
  error = disk_read(disk->id, directory_sector, directory_num_sectors,
                    (uint32_t)directory_buffer, disk->headsPerCylinder,
                    disk->sectorsPerTrack);
  return error;
}

uint32_t get_starting_cluster(Directory_Entry *entry) {
  return (entry->clusterHiBytes << 16) | entry->clusterLoBytes;
}
void print_directory_entry(Directory_Entry *entry) {
  // Calculate starting cluster from high and low bytes
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

  error = disk_read(disk->id, fat_sector, fat_num_sectors, (uint32_t)fat_buffer,
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
                      (uint32_t)output_buffer, disk->headsPerCylinder,
                      disk->sectorsPerTrack);
    output_buffer += fat12_bpb->sectorsPerCluster * fat12_bpb->bytesPerSector;
    current_cluster = get_next_cluster(FAT, current_cluster);
    printf("addr %x\n", output_buffer);
  } while (!error && current_cluster < 0xFF8);

  return error;
}

typedef void (*KernelStart)();
void __attribute__((section(".text.main"))) main() {
  FAT12_BPB fat12_bpb;
  DISK disk;
  int error;
  disk.id = DRIVE_ID;

  clear_screen();
  error = x86_get_disk_params(disk.id, &disk.drive_type, &disk.cylinders,
                              &disk.sectorsPerTrack, &disk.headsPerCylinder);
  if (error)
    printf("failed ret\n");
  fat12_bpb = *(FAT12_BPB *)boot_sect_address;

  read_directory_buffer(&fat12_bpb, &disk, DIRECTORY_BUFFER);
  Directory_Entry *entry =
      find_file("KERNEL  BIN", DIRECTORY_BUFFER, fat12_bpb.maxRootDirEntries);

  print_directory_entry(entry);

  read_FAT(FAT_BUFFER, &disk, &fat12_bpb);
  read_file(entry, &disk, &fat12_bpb, (uint8_t *)KERNEL_BUFFER, FAT_BUFFER);

  memcpy(KERNEL_LOAD_ADDR, KERNEL_BUFFER, entry->fileSize);
  KernelStart kernelStart = (KernelStart)KERNEL_LOAD_ADDR;
  kernelStart();

  for (;;)
    ;
  return;
}
