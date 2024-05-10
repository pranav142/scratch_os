#include "stage2.h"
#include "memdefs.h"
#include "paging.h"

typedef void (*KernelStart)();

void __attribute__((section(".entry"))) start() {
  FAT12_BPB fat12_bpb;
  MemoryInfo mem_info;
  DISK disk;
  int error;

  disk.id = DRIVE_ID;

  clear_screen();

  error = x86_get_disk_params(disk.id, &disk.drive_type, &disk.cylinders,
                              &disk.sectorsPerTrack, &disk.headsPerCylinder);
  if (error) {
    printf("failed ret\n");
    goto End;
  }

  read_E820_Memory_Info(&mem_info);

  fat12_bpb = *(FAT12_BPB *)BOOT_SECT_ADDRESS;

  error = read_directory_buffer(&fat12_bpb, &disk, DIRECTORY_BUFFER);
  if (error) {
    printf("Failed to read root directory\n");
    goto End;
  }

  Directory_Entry *entry =
      find_file("KERNEL  BIN", DIRECTORY_BUFFER, fat12_bpb.maxRootDirEntries);
  if (entry == 0) {
    printf("Failed to find file\n");
    goto End;
  }

  error = read_FAT(FAT_BUFFER, &disk, &fat12_bpb);
  if (error) {
    printf("Failed to read FAT\n");
    goto End;
  }

  error =
      read_file(entry, &disk, &fat12_bpb, (uint8_t *)KERNEL_BUFFER, FAT_BUFFER);
  if (error) {
    printf("Failed to read file\n");
    goto End;
  }

  memcpy(KERNEL_LOAD_ADDR, KERNEL_BUFFER, entry->fileSize);
  initialize_paging();

  KernelStart kernelStart = (KernelStart)KERNEL_VIRTUAL_ADDR;
  printf("kernel size: %x\n", entry->fileSize);
  printf("accessing kernel at addr: %x\n", kernelStart);
  kernelStart(mem_info);

End:
  for (;;)
    ;
  return;
}
