#include "mem.h"
#include "isr.h"

PageDirectory *kernel_page_dir = (PageDirectory *)0xFFFFF000;
static uint32_t *g_physical_memory_bitmap = 0;
static uint32_t g_bitmap_size = 0;

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

static void enable_paging(PageDirectory *page_dir_pa) {
  PageDirectoryEntry pd_addr = (uintptr_t)page_dir_pa->entries;

  __asm__ volatile("mov %0, %%cr3\n"
                   "mov %%cr0, %%eax\n"
                   "or $0x80000000, %%eax\n"
                   "mov %%eax, %%cr0\n"
                   :
                   : "r"(pd_addr)
                   : "eax");
}

static bool is_paging() {
  uint32_t cr0;
  __asm__ volatile("mov %%cr0, %0\n"

                   : "=r"(cr0)
                   :
                   :);

  return (cr0 & 0x80000000) != 0;
}

uintptr_t align_to_4kb(uintptr_t addr) { return (addr + 0xFFF) & ~0xFFF; }

static void flush_tlb(void) {
  uint32_t cr3;
  __asm__ volatile("mov %%cr3, %0;"
                   "mov %0, %%cr3;"
                   : "=r"(cr3)
                   :
                   :);
}

static void invplg(void *m) {
  asm volatile("invlpg (%0)" : : "r"(m) : "memory");
}

bool vmm_map_page(uintptr_t virtual_addr, uintptr_t physical_addr) {
  uint32_t pdi = PD_INDEX(virtual_addr);
  uint32_t pti = PT_INDEX(virtual_addr);

  PageDirectoryEntry *pd_entry = &kernel_page_dir->entries[pdi];
  PageTable *page_table_va = (PageTable *)PT_VIRTUAL_ADDR(pdi);

  if (!TEST_FLAG(pd_entry, PDE_PRESENT)) {
    PageTable *new_page_table = (PageTable *)alloc_block();
    PageDirectoryEntry new_pd_entry = 0;
    PageTableEntry new_pt_entry = 0;

    SET_ADDR(&new_pd_entry, (uintptr_t)new_page_table);
    SET_FLAG(&new_pd_entry, PDE_PRESENT | PDE_READ_WRITE);
    kernel_page_dir->entries[pdi] = new_pd_entry;

    invplg(page_table_va);
    memset(page_table_va, 0, sizeof(PageTable));

    SET_ADDR(&new_pt_entry, physical_addr);
    SET_FLAG(&new_pt_entry, PTE_PRESENT | PTE_READ_WRITE);
    page_table_va->entries[pti] = new_pt_entry;
  } else {
    PageTableEntry *pt_entry = &page_table_va->entries[pti];
    if (TEST_FLAG(pt_entry, PTE_PRESENT)) {
      printf("tried to map a already mapped page\n");
      return false;
    }

    SET_ADDR(pt_entry, physical_addr);
    SET_FLAG(pt_entry, PTE_PRESENT | PTE_READ_WRITE);
  }

  return true;
}

// unmaps the page but doesn't deallocate physical memory
void vmm_unmap_page(uintptr_t virtual_address) {
  uint32_t pdi = PD_INDEX(virtual_address);
  uint32_t pti = PT_INDEX(virtual_address);

  PageDirectoryEntry *pd_entry = &kernel_page_dir->entries[pdi];
  PageTable *page_table_va = (PageTable *)PT_VIRTUAL_ADDR(pdi);

  if (TEST_FLAG(pd_entry, PDE_PRESENT)) {
    PageTableEntry *pt_entry = &page_table_va->entries[pti];
    if (TEST_FLAG(pt_entry, PTE_PRESENT)) {
      void *frame_addr = (void *)GET_ADDR(pt_entry);
      *pt_entry = 0;

      invplg((void *)virtual_address);
    }
  }
}

// unmaps and de allocates underlying memory
void vmm_free_page(uintptr_t virtual_address) {
  uint32_t pdi = PD_INDEX(virtual_address);
  uint32_t pti = PT_INDEX(virtual_address);

  PageDirectoryEntry *pd_entry = &kernel_page_dir->entries[pdi];
  PageTable *page_table_va = (PageTable *)PT_VIRTUAL_ADDR(pdi);

  if (TEST_FLAG(pd_entry, PDE_PRESENT)) {
    PageTableEntry *pt_entry = &page_table_va->entries[pti];
    if (TEST_FLAG(pt_entry, PTE_PRESENT)) {
      void *frame_addr = (void *)GET_ADDR(pt_entry);
      *pt_entry = 0;

      invplg((void *)virtual_address);
      free_block(frame_addr);
    }
  }
}

PageDirectory *allocate_page_directory(uintptr_t page_directory_va) {
  PageDirectory *page_directory_pa = (PageDirectory *)alloc_block();
  if (!vmm_map_page((uintptr_t)page_directory_va, (uintptr_t)page_directory_pa))
    return NULL;
  memset((void *)page_directory_va, 0, sizeof(PageDirectory));
  return page_directory_pa;
}

PageTable *allocate_page_table(uintptr_t page_table_va) {
  PageTable *page_table_pa = (PageTable *)alloc_block();
  if (!vmm_map_page((uintptr_t)page_table_va, (uintptr_t)page_table_pa))
    return NULL;
  memset((void *)page_table_va, 0, sizeof(PageDirectory));
  return page_table_pa;
}

void page_interrupt_handler(Registers *regs) {}

void initialize_virtual_memory_manager() {
  PageDirectory *page_directory_va = (PageDirectory *)0x500000;
  PageDirectory *page_directory_pa =
      allocate_page_directory((uintptr_t)page_directory_va);
  if (!page_directory_pa)
    return;

  PageTable *kernel_map_va = (PageTable *)0x501000;
  PageTable *kernel_map_pa = allocate_page_table((uintptr_t)kernel_map_va);
  if (!page_directory_pa)
    return;

  PageTable *identity_map_va = (PageTable *)0x502000;
  PageTable *identity_map_pa = allocate_page_table((uintptr_t)identity_map_va);
  if (!identity_map_pa)
    return;

  for (int i = 0, physical_addr = (uintptr_t)KERNEL_LOAD_ADDR,
           virtual_addr = (uintptr_t)KERNEL_VIRTUAL_ADDR;
       i < PAGES_PER_TABLE;
       i += 1, physical_addr += 4096, virtual_addr += 4096) {
    PageTableEntry page = 0;
    SET_ADDR(&page, physical_addr);
    SET_FLAG(&page, PTE_PRESENT | PTE_READ_WRITE);
    kernel_map_va->entries[PT_INDEX(virtual_addr)] = page;
  };

  for (int i = 0, physical_addr = 0, virtual_addr = 0; i < PAGES_PER_TABLE;
       i += 1, physical_addr += 4096, virtual_addr += 4096) {
    PageTableEntry page = 0;
    SET_ADDR(&page, physical_addr);
    SET_FLAG(&page, PTE_PRESENT | PTE_READ_WRITE);
    identity_map_va->entries[PT_INDEX(virtual_addr)] = page;
  }

  PageDirectoryEntry identity_map_entry = 0;
  SET_ADDR(&identity_map_entry, (uintptr_t)identity_map_pa);
  SET_FLAG(&identity_map_entry, PDE_PRESENT | PDE_READ_WRITE);
  page_directory_va->entries[PD_INDEX(0)] = identity_map_entry;

  PageDirectoryEntry kernel_map_entry = 0;
  SET_ADDR(&kernel_map_entry, (uintptr_t)kernel_map_pa);
  SET_FLAG(&kernel_map_entry, PDE_PRESENT | PDE_READ_WRITE);
  page_directory_va->entries[PD_INDEX((uintptr_t)KERNEL_VIRTUAL_ADDR)] =
      kernel_map_entry;

  // recursively map the page directory to itself
  PageDirectoryEntry self_ref_entry = 0;
  SET_ADDR(&self_ref_entry, (uintptr_t)page_directory_pa);
  SET_FLAG(&self_ref_entry, PDE_PRESENT | PDE_READ_WRITE);
  page_directory_va->entries[1023] = self_ref_entry;

  // unmap temprary mappings
  vmm_unmap_page((uintptr_t)page_directory_va);
  vmm_unmap_page((uintptr_t)kernel_map_va);
  vmm_unmap_page((uintptr_t)identity_map_va);

  flush_tlb();
  enable_paging(page_directory_pa);
}
