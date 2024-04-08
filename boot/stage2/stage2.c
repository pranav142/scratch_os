#include "stage2.h"
#include "FAT.h"

#define boot_sect_address 0x7e00
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

  if (!disk_reset(0))
    printf("failed to reset disk\n");

  if (!disk_read(0, 1, 1, 0, 0, 0, boot_sect_address))
    printf("failed to read disk\n");

  fat12_bpb = *(FAT12_BPB *)boot_sect_address;
  printf("\n\nFAT FILE SYSTEM INFO:\n");
  printFAT12BPB(&fat12_bpb);

  //  read file system for the kernel
  //  startup the kernel
  for (;;)
    ;
  return;
}
