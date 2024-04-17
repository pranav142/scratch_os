#include "mem.h"
#include <stdint.h>

void read_E820_Memory_Info(MemoryInfo *mem_info_out) {
  mem_info_out->num_entries = *E820_NUM_ENTRIES;
  mem_info_out->entries = E820_ENTRIES_ADDR;
}
