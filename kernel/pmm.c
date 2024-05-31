#include "pmm.h"

static const int BITMAP_BLOCKS_PER_BYTE = 8;
static const int BITMAP_BLOCK_SIZE = 4096;
static const int BLOCKS_PER_INDEX = 32;
static uint32_t *g_physical_memory_bitmap = 0;
static uint32_t g_bitmap_size = 0;

static inline void mark_block_free(uint32_t block) {
  g_physical_memory_bitmap[block / BLOCKS_PER_INDEX] &=
      ~(1 << (block % BLOCKS_PER_INDEX));
}

static inline void mark_block_used(uint32_t block) {
  g_physical_memory_bitmap[block / BLOCKS_PER_INDEX] |=
      (1 << (block % BLOCKS_PER_INDEX));
}

static inline bool is_block_used(uint32_t block) {
  return g_physical_memory_bitmap[block / BLOCKS_PER_INDEX] &
         (1 << (block % BLOCKS_PER_INDEX));
}

static uint32_t calculate_bitmap_size(MemoryInfo *mem_info) {
  E820MemoryMapEntry *entry = mem_info->entries;
  uint32_t num_entries = mem_info->num_entries;
  uint32_t bitmap_size = 0;

  for (int i = 0; i < num_entries; i++) {
    bitmap_size += entry->length;
    entry++;
  }

  return bitmap_size / BITMAP_BLOCK_SIZE;
}

static inline int calculate_num_blocks(size_t size) {
  return ((size / BITMAP_BLOCK_SIZE) + 1);
}

static void mark_physical_memory_region_free(uintptr_t base_addr,
                                             uint32_t size) {
  int align = base_addr / BITMAP_BLOCK_SIZE;
  int blocks = calculate_num_blocks(size);

  for (; blocks > 0; blocks--) {
    mark_block_free(align++);
  }

  mark_block_used(0);
}

static void mark_physical_memory_region_used(uintptr_t base_addr,
                                             uint32_t size) {
  int align = base_addr / BITMAP_BLOCK_SIZE;
  int blocks = calculate_num_blocks(size);

  for (; blocks > 0; blocks--) {
    mark_block_used(align++);
  }
}

static uint32_t get_num_free_blocks() {
  uint32_t free_block_count = 0;
  for (size_t i = 0; i < g_bitmap_size; i++) {
    if (!is_block_used(i)) {
      free_block_count++;
    }
  }
  return free_block_count;
}

void initialize_physical_memory_manager(MemoryInfo *mem_info,
                                        uintptr_t bitmap_addr,
                                        MemoryRegion *reserved_regions,
                                        size_t num_reserved_regions) {
  g_bitmap_size = calculate_bitmap_size(mem_info);
  g_physical_memory_bitmap = (uint32_t *)bitmap_addr;

  memset(g_physical_memory_bitmap, 0xff, g_bitmap_size);

  for (size_t i = 0; i < mem_info->num_entries; i++) {
    E820MemoryMapEntry *region = &mem_info->entries[i];
    if (region->type == E820_USABLE) {
      mark_physical_memory_region_free((uintptr_t)region->base_addr,
                                       region->length);
    } else {
      mark_physical_memory_region_used((uintptr_t)region->base_addr,
                                       region->length);
    }
  }

  mark_physical_memory_region_used((uintptr_t)bitmap_addr, g_bitmap_size);

  for (size_t i = 0; i < num_reserved_regions; i++) {
    mark_physical_memory_region_used(reserved_regions[i].start_address,
                                     reserved_regions[i].length);
  }
}

static int find_first_free_sequence(size_t num_blocks) {
  size_t consecutive_free = 0;

  for (size_t i = 0; i < g_bitmap_size; i++) {
    if (!is_block_used(i)) {
      consecutive_free++;
      if (consecutive_free == num_blocks) {
        return i - num_blocks + 1;
      }
    } else {
      consecutive_free = 0;
    }
  }

  return -1;
}

static int find_first_free_block() {
  for (size_t i = 0; i < g_bitmap_size; i++) {
    if (!is_block_used(i)) {
      return i;
    }
  }

  return -1;
}
void *alloc_block() {
  int block = find_first_free_block();
  uintptr_t block_address;
  if (block == -1) {
    return NULL;
  }
  mark_block_used(block);

  block_address = block * BITMAP_BLOCK_SIZE;
  return (void *)(block_address);
}

void free_block(void *addr) {
  mark_block_free((uintptr_t)addr / BITMAP_BLOCK_SIZE);
}

void *alloc_blocks(uint32_t size) {
  uintptr_t allocated_address;

  if (get_num_free_blocks() < size) {
    return NULL;
  }

  int blocks = calculate_num_blocks(size);
  int starting_block = find_first_free_sequence(blocks);
  if (starting_block == -1)
    return NULL;

  allocated_address = starting_block * BITMAP_BLOCK_SIZE;

  mark_physical_memory_region_used(allocated_address, size);

  return (void *)(allocated_address);
}

void free_blocks(void *addr, size_t size) {
  mark_physical_memory_region_free((uintptr_t)addr, size);
}

size_t get_used_physical_memory() {
  int used_blocks = g_bitmap_size - get_num_free_blocks();
  return used_blocks * BITMAP_BLOCK_SIZE;
}

size_t get_available_physical_memory() {
  int free_blocks = get_num_free_blocks();
  return free_blocks * BITMAP_BLOCK_SIZE;
}

uint32_t get_bitmap_size() { return g_bitmap_size; }

uintptr_t get_bitmap_addr() { return (uintptr_t)g_physical_memory_bitmap; }
