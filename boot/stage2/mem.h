#pragma once

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

#define E820_NUM_ENTRIES (int *)0x500
#define E820_ENTRIES_ADDR (E820MemoryMapEntry *)0x504

void read_E820_Memory_Info(MemoryInfo *mem_info_out);
