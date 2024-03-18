#include "mem.h"

#include <stdint.h>

void read_memory_map() {
  E820MemoryMapEntry *entries = (E820MemoryMapEntry *)MEMORY_MAP_ADDR;
  uint16_t num_entries = *(uint16_t *)MEMORY_MAP_COUNT_ADDR;
  printf("num_entries %d\n", num_entries);

  for (int i = 0; i < num_entries; i++) {
    printf("Entry %d:Type = %x ", i, entries[i].type);
    printf(" address = %x ", entries[i].base_addr);
    printf(" length = %x \n", entries[i].length);
  }
}
