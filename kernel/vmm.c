#include "vmm.h"

static const PageTable *page_table_base = (PageTable *)0xFFC00000;
static PageDirectory *page_dir_base = (PageDirectory *)0xFFFFF000;
static PageDirectory *g_current_page_dir_pa = 0;

static inline uint32_t get_page_dir_index(uintptr_t virtual_addr) {
  return (virtual_addr >> 22) & 0x3FF;
}

static inline uint32_t get_page_table_index(uintptr_t virtual_addr) {
  return (virtual_addr >> 12) & 0x3FF;
}

static inline uintptr_t get_page_table_virtual_addr(uint32_t pdi) {
  return ((uintptr_t)page_table_base + (pdi * 0x1000));
}

static void enable_paging() {
  PageDirectoryEntry pd_addr = (uintptr_t)g_current_page_dir_pa->entries;

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

static inline uintptr_t align_to_4kb(uintptr_t addr) { return addr & ~0xFFF; }

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

static void switch_pdbr(PageDirectory *new_pdbr) {
  asm volatile("mov %0, %%cr3" : : "r"((uintptr_t)new_pdbr) : "memory");
}

bool vmm_map_address(uintptr_t virtual_addr, uintptr_t physical_addr) {
  uint32_t pdi = get_page_dir_index(virtual_addr);
  uint32_t pti = get_page_table_index(virtual_addr);

  PageDirectoryEntry *pd_entry = &page_dir_base->entries[pdi];
  PageTable *page_table_va = (PageTable *)get_page_table_virtual_addr(pdi);

  if (!TEST_FLAG(pd_entry, PDE_PRESENT)) {
    PageTable *new_page_table = (PageTable *)alloc_block();
    PageDirectoryEntry new_pd_entry = 0;
    PageTableEntry new_pt_entry = 0;

    SET_ADDR(&new_pd_entry, (uintptr_t)new_page_table);
    SET_FLAG(&new_pd_entry, PDE_PRESENT | PDE_READ_WRITE);
    page_dir_base->entries[pdi] = new_pd_entry;

    invplg(page_table_va);
    memset(page_table_va, 0, sizeof(PageTable));
  }

  PageTableEntry *pt_entry = &page_table_va->entries[pti];
  if (TEST_FLAG(pt_entry, PTE_PRESENT)) {
    // printf("tried to map a already mapped page\n");
    return false;
  }

  SET_ADDR(pt_entry, physical_addr);
  SET_FLAG(pt_entry, PTE_PRESENT | PTE_READ_WRITE);

  return true;
}

void vmm_unmap_address(uintptr_t virtual_address) {
  uint32_t pdi = get_page_dir_index(virtual_address);
  uint32_t pti = get_page_table_index(virtual_address);

  PageDirectoryEntry *pd_entry = &page_dir_base->entries[pdi];
  PageTable *page_table_va = (PageTable *)get_page_table_virtual_addr(pdi);

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
  if (!vmm_map_address((uintptr_t)page_directory_va,
                       (uintptr_t)page_directory_pa))
    return NULL;
  memset((void *)page_directory_va, 0, sizeof(PageDirectory));
  return page_directory_pa;
}

PageTable *allocate_page_table(uintptr_t page_table_va) {
  PageTable *page_table_pa = (PageTable *)alloc_block();
  if (!vmm_map_address((uintptr_t)page_table_va, (uintptr_t)page_table_pa))
    return NULL;
  memset((void *)page_table_va, 0, sizeof(PageTable));
  return page_table_pa;
}

void page_fault_handler(Registers *regs) {
  uintptr_t faulting_address;
  asm volatile("mov %%cr2, %0" : "=r"(faulting_address));

  uint32_t error_code = regs->error;

  int present = !(error_code & PAGE_PROTECTION_VIOLATION);
  int rw = error_code & INVALID_WRITE_ACCESS;
  int us = error_code & PRIVILIGE_VIOLATION;
  int reserved = error_code & RESERVED_BIT_VIOLATION;
  int id = error_code & INSTRUCTION_FETCH_VIOLATION;

  printf("KERNEL PANIC\n");
  printf("Page fault at %x (present: %d, rw: %d, us: %d, reserved: %d, id: "
         "%d)\n",
         faulting_address, present, rw, us, reserved, id);
  kernel_Panic();
}

void switch_current_page_directory(PageDirectory *page_directory_pa) {
  if (!page_directory_pa)
    return;
  g_current_page_dir_pa = page_directory_pa;
  switch_pdbr(g_current_page_dir_pa);
}

void recursively_map_page_directory(PageDirectory *page_directory_pa,
                                    PageDirectory *page_directory_va) {
  PageDirectoryEntry self_ref_entry = 0;
  SET_ADDR(&self_ref_entry, (uintptr_t)page_directory_pa);
  SET_FLAG(&self_ref_entry, PDE_PRESENT | PDE_READ_WRITE);
  page_directory_va->entries[1023] = self_ref_entry;
}

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
    kernel_map_va->entries[get_page_table_index(virtual_addr)] = page;
  };

  for (int i = 0, physical_addr = 0, virtual_addr = 0; i < PAGES_PER_TABLE;
       i += 1, physical_addr += 4096, virtual_addr += 4096) {
    PageTableEntry page = 0;
    SET_ADDR(&page, physical_addr);
    SET_FLAG(&page, PTE_PRESENT);
    identity_map_va->entries[get_page_table_index(virtual_addr)] = page;
  }

  PageDirectoryEntry identity_map_entry = 0;
  SET_ADDR(&identity_map_entry, (uintptr_t)identity_map_pa);
  SET_FLAG(&identity_map_entry, PDE_PRESENT);
  page_directory_va->entries[get_page_dir_index(0)] = identity_map_entry;

  PageDirectoryEntry kernel_map_entry = 0;
  SET_ADDR(&kernel_map_entry, (uintptr_t)kernel_map_pa);
  SET_FLAG(&kernel_map_entry, PDE_PRESENT | PDE_READ_WRITE);
  page_directory_va
      ->entries[get_page_dir_index((uintptr_t)KERNEL_VIRTUAL_ADDR)] =
      kernel_map_entry;

  recursively_map_page_directory(page_directory_pa, page_directory_va);

  flush_tlb();
  switch_current_page_directory(page_directory_pa);

  set_ISR_handler(PAGE_FAULT_INTERRUPT_NUMBER, page_fault_handler);
}

void memory_test() {
  uintptr_t START_ADDR = 0x500000;
  uintptr_t END_ADDR = 0x510000;
  for (size_t virtual_addr = START_ADDR; virtual_addr < END_ADDR;
       virtual_addr++) {
    printf("%x\n", virtual_addr);

    void *phys_addr = alloc_block();
    if (phys_addr == NULL)
      printf("ran out of physical memory\n");

    if (!vmm_map_address(virtual_addr, (uintptr_t)phys_addr)) {
      free_block(phys_addr);
      continue;
    }
    char test_var = *(char *)virtual_addr;

    vmm_unmap_address(virtual_addr);
    void *new_phys_addr = alloc_block();
    if (new_phys_addr != phys_addr) {
      printf("failed\n");
      return;
    }
    free_block(new_phys_addr);
  }

  printf("memory test passed\n");
}
