#pragma once

#include "../../drivers/screen.h"
#include "../../utils/memtools.h"
#include "memdefs.h"
#include <stdint.h>

#define PD_INDEX(addr) (addr >> 22)
#define PT_INDEX(addr) ((addr >> 12) & 0x3FF)
#define PT_OFFSET(addr) (addr & 0xFFF)
#define SET_FLAG(entry, flag) (*entry |= flag)
#define CLEAR_FLAG(entry, flag) (*entry &= ~flag)
#define TEST_FLAG(entry, flag) (*entry & flag)
#define SET_ADDR(entry, addr)                                                  \
  (*entry = (*entry & ~0xFFFFF000) | (addr & 0xFFFFF000))
#define GET_ADDR(entry) (*entry & 0xFFFFF000)

#define TABLES_PER_DIRECTORY 1024
#define PAGES_PER_TABLE 1024

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
} PAGE_TABLE_FLAGS;

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
} PAGE_DIR_FLAGS;

typedef struct {
  PageDirectoryEntry entries[TABLES_PER_DIRECTORY];
} PageDirectory;

typedef struct {
  PageTableEntry entries[PAGES_PER_TABLE];
} PageTable;

static void enable_paging(PageDirectory *page_directory);
static bool is_paging();
void initialize_paging();
