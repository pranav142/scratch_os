#pragma once

#include "../drivers/ports.h"
#include "../drivers/screen.h"
#include "../utils/memdefs.h"
#include "../utils/memtools.h"
#include "isr.h"
#include "pmm.h"
#include <stddef.h>
#include <stdint.h>

#define SET_FLAG(entry, flag) (*entry |= flag)
#define CLEAR_FLAG(entry, flag) (*entry &= ~flag)
#define TEST_FLAG(entry, flag) (*entry & flag)
#define SET_ADDR(entry, addr)                                                  \
  (*(entry) = (*(entry) & ~0xFFFFF000) | ((addr) & 0xFFFFF000))
#define GET_ADDR(entry) (*entry & 0xFFFFF000)

#define TABLES_PER_DIRECTORY 1024
#define PAGES_PER_TABLE 1024
#define PAGE_FAULT_INTERRUPT_NUMBER 0xE

#define PAGE_SIZE 4096

typedef uint32_t PageTableEntry;
typedef uint32_t PageDirectoryEntry;

typedef enum {
  PTE_PRESENT = 0x01,
  PTE_READ_WRITE = 0x02,
  PTE_USER = 0x04,
  PTE_WRITE_THROUGH = 0x08,
  PTE_CACHE_DISABLE = 0x10,
  PTE_ACCESSED = 0x20,
  PTE_DIRTY = 0x40,
  PTE_PAT = 0x80,
  PTE_GLOBAL = 0x100,
  PTE_FRAME = 0xFFFFF000, // bits 12+ first twenty bits of physical addr
} PageTableFlags;

typedef enum {
  PDE_PRESENT = 0x01,
  PDE_READ_WRITE = 0x02,
  PDE_USER = 0x04,
  PDE_WRITE_THROUGH = 0x08,
  PDE_CACHE_DISABLE = 0x10,
  PDE_ACCESSED = 0x20,
  PDE_DIRTY = 0x40,       // 4MB entry only
  PDE_PAGE_SIZE = 0x80,   // 0 = 4KB page, 1 = 4MB page
  PDE_GLOBAL = 0x100,     // 4MB entry only
  PDE_PAT = 0x2000,       // 4MB entry only
  PDE_FRAME = 0xFFFFF000, // bits 12+ first 20 bits of table address
} PageDirFlags;

typedef enum {
  PAGE_PROTECTION_VIOLATION = 0x01, // not set -> not present page
  INVALID_WRITE_ACCESS = 0x02,      // not set -> invalid read access
  PRIVILIGE_VIOLATION = 0x4,
  RESERVED_BIT_VIOLATION = 0x8,
  INSTRUCTION_FETCH_VIOLATION = 0x10,
  PROTECTION_KEY_VIOLATION = 0x20,
  SHADOW_STACK_ACCESS = 0x30,
  SOFTWARE_GUARD_VIOLATION = 0x40,
} PageFaultErrors;

typedef struct {
  PageDirectoryEntry entries[TABLES_PER_DIRECTORY];
} PageDirectory;

typedef struct {
  PageTableEntry entries[PAGES_PER_TABLE];
} PageTable;

static inline uint32_t get_page_dir_index(uintptr_t virtual_addr);
static inline uint32_t get_page_table_index(uintptr_t virtual_addr);
static inline uintptr_t get_page_table_virtual_addr(uint32_t pdi);
static void enable_paging();
static bool is_paging();
static inline uintptr_t align_to_4kb(uintptr_t addr);
static void flush_tlb(void);
static void invplg(void *m);
static void switch_pdbr(PageDirectory *new_pdbr);
void initialize_virtual_memory_manager();
bool vmm_map_address(uintptr_t virtual_addr, uintptr_t physical_addr);
void vmm_unmap_address(uintptr_t virtual_address);
void memory_test();
