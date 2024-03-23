#include "mem.h"

MemoryInfo g_MemInfo;
E820MemoryMapEntry g_MemoryMapEntries[MAX_ENTRIES];
uint16_t g_PhysicalBitmap[BITMAP_SIZE];
PageTableEntry g_PageTable[MAX_PAGE_ENTRIES];
PageDirectoryEntry g_PageDirectory[MAX_PAGE_ENTRIES];

void read_memory_map() {
  E820MemoryMapEntry *entry = (E820MemoryMapEntry *)MEMORY_MAP_ADDR;
  uint16_t num_entries = *(uint16_t *)MEMORY_MAP_COUNT_ADDR;

  if (num_entries > MAX_ENTRIES) {
    printf("Not enough space in global memory map to store all entries\n");
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

  for (int i = 0; i < g_MemInfo.num_entries; i++) {
    printf("Entry: %d, Address: %x, Length: %x, Type: %x\n", i,
           (int)(entry->base_addr), (int)(entry->length), (int)(entry->type));
    entry++;
  }
}

void initialize_physical_bit_map() {
  for (int i = 0; i < BITMAP_SIZE; i++) {
    g_PhysicalBitmap[i] = 0;
  }
}

void mark_physical_block_used(int block) {
  if (block > BITMAP_BLOCK_COUNT) {
    printf("physical block out of index\n");
    return;
  }

  g_PhysicalBitmap[block / BLOCKS_PER_INDEX] |=
      (1 << (block % BLOCKS_PER_INDEX));
}

void mark_physical_block_unused(int block) {
  if (block > BITMAP_BLOCK_COUNT) {
    printf("physical block out of index\n");
    return;
  }

  g_PhysicalBitmap[block / BLOCKS_PER_INDEX] &=
      ~(1 << (block % BLOCKS_PER_INDEX));
}

bool block_is_used(int block) {
  return (g_PhysicalBitmap[block / BLOCKS_PER_INDEX] &
          (1 << block % BLOCKS_PER_INDEX));
}

void print_physical_bit_map(int begin, int end) {
  if (begin < 0 || end > BITMAP_SIZE) {
    printf("begin and end out of bitmap range\n");
    return;
  }
  for (int i = begin; i < end; i++) {
    printf("blocks %d - %d: %x \n", BLOCKS_PER_INDEX * i,
           BLOCKS_PER_INDEX * i + BLOCKS_PER_INDEX, g_PhysicalBitmap[i]);
  }
}

int find_free_block() {
  for (int i = 0; i < BITMAP_BLOCK_COUNT; i++) {
    if (!(block_is_used(i))) {
      return i;
    }
  }

  return -1;
}

void *allocate_block() {
  int block = find_free_block();
  if (block == -1) {
    return 0;
  }
  mark_physical_block_used(block);
  return (void *)(BLOCK_TO_ADDR(block));
}

void free_block(void *mem_addr) {
  int block = ADDR_TO_BLOCK((int)mem_addr);
  mark_physical_block_unused(block);
} 


