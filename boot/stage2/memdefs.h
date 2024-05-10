#pragma once

#define BOOT_SECT_ADDRESS 0x7c00
#define BOOT_SECTOR 0
#define BOOT_SECTOR_LENGTH 1

#define DIRECTORY_BUFFER ((Directory_Entry *)0x7e00)
#define FAT_BUFFER ((uint8_t *)0x9b4c)

#define DRIVE_ID 0

#define KERNEL_BUFFER ((uint8_t *)0x30000)
#define KERNEL_LOAD_ADDR (void *)0x100000
#define KERNEL_VIRTUAL_ADDR (void *)0xC0000000

#define IDENTITY_MAP_TABLE_ADDR 0x60000
#define KERNEL_MAP_TABLE_ADDR 0x62000
#define PAGING_DIRECTORY_ADDR 0x64000
