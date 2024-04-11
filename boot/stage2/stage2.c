#include "stage2.h"
#include "FAT.h"

#define boot_sect_address 0x7e00
#define BOOT_SECTOR 0
#define BOOT_SECTOR_LENGTH 1
#define DRIVE_ID 0

void __attribute__((section(".text.main"))) main() {
  FAT12_BPB fat12_bpb;
  DISK disk;
  uint8_t drive_type;
  uint16_t cylinders, sectors, heads;
  disk.id = DRIVE_ID;

  clear_screen();
  if (!get_disk_params(disk.id, &disk.drive_type, &disk.cylinders,
                       &disk.sectors, &disk.heads))
    printf("failed ret\n");

  printf("DRIVE INFO:\ndrive type: %d, cylinders: %d, sectors: %d, heads: %d\n",
         disk.drive_type, disk.cylinders, disk.sectors, disk.heads);

  if (!disk_read(disk.id, BOOT_SECTOR, BOOT_SECTOR_LENGTH, boot_sect_address,
                 disk.cylinders, disk.sectors))
    printf("failed to read disk\n");

  fat12_bpb = *(FAT12_BPB *)boot_sect_address;
  printf("\n\nFAT FILE SYSTEM INFO:\n");
  printFAT12BPB(&fat12_bpb);

  //  read file system for the kernel
  // reading directories
  int directory_sector =
      fat12_bpb.reservedSectors + fat12_bpb.numFATs * fat12_bpb.sectorsPerFAT;
  int directory_num_sectors =
      (fat12_bpb.maxRootDirEntries * DIRECTORY_ENTRY_BYTE_SIZE) /
      fat12_bpb.bytesPerSector;

  printf("directory sector: %d, directory_size: %d \n", directory_sector,
         directory_num_sectors);

  // read fat table
  //  startup the kernel
  for (;;)
    ;
  return;
}
