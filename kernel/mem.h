#pragma once

#include <stdint.h>
#include "../drivers/screen.h"


typedef struct {
  uint64_t base_addr; 
  uint64_t length;    
  uint32_t type; 
  uint32_t ascii; 
} __attribute__((packed)) E820MemoryMapEntry;

typedef struct { 
  int num_entries;
  E820MemoryMapEntry* entries;
} MemoryInfo;

enum E820MemoryBlockType 
{
    E820_USABLE = 1,
    E820_RESERVED = 2,
    E820_ACPI_RECLAIMABLE = 3,
    E820_ACPI_NVS = 4,
    E820_BAD_MEMORY = 5,
};

#define MEMORY_MAP_COUNT_ADDR 0x8000
#define MEMORY_MAP_ADDR 0x8004
#define MAX_ENTRIES 16

void read_memory_map();
void print_memory_map();
