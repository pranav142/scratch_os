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

void mark_memory_free(uint32_t bit) {
  physical_memory_bitmap[bit / BLOCKS_PER_INDEX] &=
      ~(1 << (bit % BLOCKS_PER_INDEX));
}

void mark_memory_used(uint32_t bit) {
  physical_memory_bitmap[bit / BLOCKS_PER_INDEX] |=
      (1 << (bit % BLOCKS_PER_INDEX));
}

bool test_memory_map(uint32_t bit) {
  return physical_memory_bitmap[bit / BLOCKS_PER_INDEX] &
         (1 << (bit % BLOCKS_PER_INDEX));
}

uint32_t calculate_bitmap_size(MemoryInfo *mem_info) {
  E820MemoryMapEntry *entry = mem_info->entries;
  uint32_t num_entries = mem_info->num_entries;
  uint32_t bitmap_size = 0;

  for (int i = 0; i < num_entries; i++) {
    bitmap_size += entry->length;
    entry++;
  }

  return bitmap_size / BITMAP_BLOCK_SIZE;
}

int calculate_num_blocks(size_t size) {
  return ((size / BITMAP_BLOCK_SIZE) + 1);
}

void initialize_physical_memory_region(uintptr_t base_addr, uint32_t size) {
  int align = base_addr / BITMAP_BLOCK_SIZE;
  int blocks = calculate_num_blocks(size);

  for (; blocks > 0; blocks--) {
    mark_memory_free(align++);
  }

  mark_memory_used(0);
}

void deinitialize_physical_memory_region(uintptr_t base_addr, uint32_t size) {
  int align = base_addr / BITMAP_BLOCK_SIZE;
  int blocks = calculate_num_blocks(size);

  for (; blocks > 0; blocks--) {
    mark_memory_used(align++);
  }
}

uint32_t get_num_free_blocks() {
  uint32_t free_block_count = 0;
  for (size_t i = 0; i < bitmap_size; i++) {
    if (!test_memory_map(i)) {
      free_block_count++;
    }
  }
  return free_block_count;
}

// allocate memory
void initialize_physical_memory_manager(MemoryInfo *mem_info,
                                        uintptr_t bitmap_addr,
                                        MemoryRegion *reserved_regions,
                                        size_t num_reserved_regions) {
  bitmap_size = calculate_bitmap_size(mem_info);
  physical_memory_bitmap = (uint32_t *)bitmap_addr;

  memset(physical_memory_bitmap, 0xff, bitmap_size);

  for (size_t i = 0; i < mem_info->num_entries; i++) {
    E820MemoryMapEntry *region = &mem_info->entries[i];
    if (region->type == E820_USABLE) {
      initialize_physical_memory_region((uintptr_t)region->base_addr,
                                        region->length);
    } else {
      deinitialize_physical_memory_region((uintptr_t)region->base_addr,
                                          region->length);
    }
  }

  deinitialize_physical_memory_region((uintptr_t)bitmap_addr, bitmap_size);

  for (size_t i = 0; i < num_reserved_regions; i++) {
    deinitialize_physical_memory_region(reserved_regions[i].start_address,
                                        reserved_regions[i].length);
  }
}

int find_first_free_sequence(size_t num_blocks) {
  size_t consecutive_free = 0;

  for (size_t i = 0; i < bitmap_size; i++) {
    if (!test_memory_map(i)) {
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
  if (get_num_free_blocks() < size) {
    return NULL;
  }

  int blocks = calculate_num_blocks(size);
  int starting_block = find_first_free_sequence(blocks);
  if (starting_block == -1)
    return NULL;

  deinitialize_physical_memory_region(starting_block * BITMAP_BLOCK_SIZE, size);

  return (void *)(starting_block * BITMAP_BLOCK_SIZE);
}

void free_blocks(void *addr, size_t size) {
  initialize_physical_memory_region((uintptr_t)addr, size);
}

// free memory
// get free memory size
// get used memory size
//

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
