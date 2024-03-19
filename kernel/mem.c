#include "mem.h"

#include <stdint.h>

MemoryInfo g_MemInfo;
E820MemoryMapEntry g_MemoryMapEntries[MAX_ENTRIES];

void read_memory_map() {
  E820MemoryMapEntry *entry = (E820MemoryMapEntry *)MEMORY_MAP_ADDR;
  uint16_t num_entries = *(uint16_t *)MEMORY_MAP_COUNT_ADDR;

  if (num_entries > MAX_ENTRIES) {
    printf("Not enoguh space in global memory map to store all entries\n");
    return;
  }

  for (int i = 0; i < num_entries; i++) { 
    g_MemoryMapEntries[i].type = entry->type; 
    g_MemoryMapEntries[i].length = entry->length;
    g_MemoryMapEntries[i].ascii = entry->ascii;
    g_MemoryMapEntries[i].base_addr = entry->base_addr;
    entry++;
  }

  g_MemInfo.num_entries = num_entries;
  g_MemInfo.entries = g_MemoryMapEntries;
}

void print_memory_map() {
  E820MemoryMapEntry *entry = g_MemInfo.entries;
  
  // TODO: Fix Printf
  for (int i = 0; i < g_MemInfo.num_entries; i++) { 
    printf("Entry: %d ", i);
    printf("Address: %x ", entry->base_addr);
    printf("Length: %x ", entry->length);
    printf("Type: %x\n", entry->type);
    entry++;
  } 
}

