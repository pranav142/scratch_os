#include "mem.h"

PageDirectory *current_page_directory = NULL;

void print_memory_map(MemoryInfo *mem_info) {
  E820MemoryMapEntry *entry = mem_info->entries;
  int num_entries = mem_info->num_entries;

  for (int i = 0; i < num_entries; i++) {
    printf("Entry: %d, Address: %x, Length: %x, Type: %x\n", i,
           (int)entry->base_addr, (int)(entry->length), (int)(entry->type));
    entry++;
  }
}

static void mark_block_free(uint32_t block) {
  g_physical_memory_bitmap[block / BLOCKS_PER_INDEX] &=
      ~(1 << (block % BLOCKS_PER_INDEX));
}

static void mark_block_used(uint32_t block) {
  g_physical_memory_bitmap[block / BLOCKS_PER_INDEX] |=
      (1 << (block % BLOCKS_PER_INDEX));
}

static bool is_block_used(uint32_t block) {
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

static int calculate_num_blocks(size_t size) {
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

// PageTableEntry *get_page(void *virtual_address) {
//   PageDirectory *pd = current_page_directory;
//   if (pd == NULL) {
//     printf("Page Directory Not Defined\n");
//     return NULL;
//   }
//
//   PageDirectoryEntry *pd_entry =
//   &pd->entries[PD_INDEX((int)virtual_address)]; if (!TEST_FLAG(pd_entry,
//   PDE_PRESENT)) {
//     printf("Page Table Not Defined\n");
//     return NULL;
//   }
//
//   PageTable *pt = (PageTable *)GET_ADDR(pd_entry);
//
//   PageTableEntry *page = &pt->entries[PT_INDEX((int)virtual_address)];
//
//   return page;
// }
//
// void allocate_page(PageTableEntry *page) {
//   void *block = allocate_block();
//   if (!block) {
//     printf("Could not find free block\n");
//     return;
//   }
//
//   SET_ADDR(page, (int)block);
//   SET_FLAG(page, PTE_PRESENT);
// }
//
// void free_page(PageTableEntry *page) {
//   void *physical_address = (void *)GET_ADDR(page);
//   if (physical_address) {
//     free_block(physical_address);
//   }
//
//   CLEAR_FLAG(page, PTE_PRESENT);
// }
//
// void map_page(void *physical_address, void *virtual_address) {
//   PageDirectory *pd = current_page_directory;
//   if (!pd) {
//     printf("Page directory doesn't exist");
//     return;
//   }
//
//   PageDirectoryEntry *pd_entry =
//   &pd->entries[PD_INDEX((int)virtual_address)]; if (!TEST_FLAG(pd_entry,
//   PDE_PRESENT)) {
//     PageTable *page_table = (PageTable *)allocate_block();
//     if (!page_table) {
//       printf("Not enough memory to allocate page table\n");
//       return;
//     }
//
//     SET_FLAG(pd_entry, PDE_PRESENT);
//     SET_FLAG(pd_entry, PDE_READ_WRITE);
//     SET_ADDR(pd_entry, (int)page_table);
//   }
//
//   PageTable *page_table = (PageTable *)GET_ADDR(pd_entry);
//   PageTableEntry *page =
//   &page_table->entries[PT_INDEX((int)virtual_address)]; SET_ADDR(page,
//   (int)physical_address); SET_FLAG(page, PTE_PRESENT);
// }
