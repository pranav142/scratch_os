#include "paging.h"

PageDirectory *g_pageDirectory = (PageDirectory *)PAGING_DIRECTORY_ADDR;
PageTable *g_identity_map_table = (PageTable *)IDENTITY_MAP_TABLE_ADDR;
PageTable *g_kernel_map_table = (PageTable *)KERNEL_MAP_TABLE_ADDR;

static void enable_paging(PageDirectory *page_directory) {
  PageDirectoryEntry pd_addr = (uintptr_t)page_directory->entries;

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

void initialize_paging() {
  memset(g_pageDirectory, 0, sizeof(PageDirectory));
  memset(g_identity_map_table, 0, sizeof(PageTable));
  memset(g_identity_map_table, 0, sizeof(PageTable));

  for (int i = 0, physical_address = 0, virtual_address = 0;
       i < PAGES_PER_TABLE;
       i += 1, physical_address += 4096, virtual_address += 4096) {

    PageTableEntry page = 0;
    SET_ADDR(&page, physical_address);
    SET_FLAG(&page, PTE_PRESENT);
    g_identity_map_table->entries[PT_INDEX(virtual_address)] = page;
  }

  for (int i = 0, physical_address = (int)KERNEL_LOAD_ADDR,
           virtual_address = (int)KERNEL_VIRTUAL_ADDR;
       i < PAGES_PER_TABLE;
       i += 1, physical_address += 4096, virtual_address += 4096) {
    PageTableEntry page = 0;
    SET_ADDR(&page, physical_address);
    SET_FLAG(&page, PTE_PRESENT);
    g_kernel_map_table->entries[PT_INDEX(virtual_address)] = page;
  }

  PageDirectoryEntry identity_map_entry = 0;
  SET_ADDR(&identity_map_entry, (uintptr_t)g_identity_map_table);
  SET_FLAG(&identity_map_entry, PDE_PRESENT);
  SET_FLAG(&identity_map_entry, PDE_READ_WRITE);
  printf("%x\n", identity_map_entry);
  g_pageDirectory->entries[PD_INDEX(0)] = identity_map_entry;

  PageDirectoryEntry kernel_map_entry = 0;
  SET_ADDR(&kernel_map_entry, (uintptr_t)g_kernel_map_table);
  SET_FLAG(&kernel_map_entry, PDE_PRESENT);
  SET_FLAG(&kernel_map_entry, PDE_READ_WRITE);
  printf("%x\n", kernel_map_entry);
  g_pageDirectory->entries[PD_INDEX((uint32_t)KERNEL_VIRTUAL_ADDR)] =
      kernel_map_entry;

  // recursively map the page directory to itself
  PageDirectoryEntry self_ref_entry = 0;
  SET_ADDR(&self_ref_entry, (uintptr_t)g_pageDirectory);
  SET_FLAG(&self_ref_entry, PDE_PRESENT | PDE_READ_WRITE);
  g_pageDirectory->entries[1023] = self_ref_entry;

  enable_paging(g_pageDirectory);
}
