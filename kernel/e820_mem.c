#include "e820_mem.h"

void print_memory_map(MemoryInfo *mem_info) {
  E820MemoryMapEntry *entry = mem_info->entries;
  int num_entries = mem_info->num_entries;

  for (size_t i = 0; i < num_entries; i++) {
    printf("Entry: %d, Address: %x, Length: %x, Type: %x\n", i,
           (int)entry->base_addr, (int)(entry->length), (int)(entry->type));
    entry++;
  }
}
