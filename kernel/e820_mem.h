#pragma once
#include "../drivers/screen.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
  uint32_t ascii;
} __attribute__((packed)) E820MemoryMapEntry;

typedef struct {
  int num_entries;
  E820MemoryMapEntry *entries;
} MemoryInfo;

typedef enum {
  E820_USABLE = 1,
  E820_RESERVED = 2,
  E820_ACPI_RECLAIMABLE = 3,
  E820_ACPI_NVS = 4,
  E820_BAD_MEMORY = 5,
} E820MemoryBlockType;

typedef struct {
  uintptr_t start_address;
  size_t length;
} MemoryRegion;

void print_memory_map(MemoryInfo *mem_info);
