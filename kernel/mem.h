#pragma once

#include "../drivers/screen.h"
#include "../utils/memdefs.h"
#include "../utils/memtools.h"
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint64_t base_addr;
  uint64_t length;
  uint32_t type;
  uint32_t ascii;
} __attribute__((packed)) E820MemoryMapEntry;

typedef struct {
  int num_entries;
  E820MemoryMapEntry *entries;
} MemoryInfo;

enum E820MemoryBlockType {
  E820_USABLE = 1,
  E820_RESERVED = 2,
  E820_ACPI_RECLAIMABLE = 3,
  E820_ACPI_NVS = 4,
  E820_BAD_MEMORY = 5,
};

#define PD_INDEX(addr) ((addr >> 22))
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
  uintptr_t start_address;
  size_t length;
} MemoryRegion;

typedef struct {
  PageDirectoryEntry entries[TABLES_PER_DIRECTORY];
} PageDirectory;

typedef struct {
  PageTableEntry entries[PAGES_PER_TABLE];
} PageTable;

#define BITMAP_BLOCKS_PER_BYTE 8
#define BITMAP_BLOCK_SIZE 4096
#define BLOCKS_PER_INDEX 32

#define SELF_MAP_BASE 0xFFC00000

void initialize_physical_memory_manager(MemoryInfo *mem_info,
                                        uintptr_t bitmap_addr,
                                        MemoryRegion *reserved_regions,
                                        size_t num_reserved_regions);
void *alloc_blocks(uint32_t size);
void free_blocks(void *addr, size_t size);
void print_memory_map(MemoryInfo *mem_info);
static void mark_block_free(uint32_t block);
static void mark_block_used(uint32_t block);
static bool is_block_used(uint32_t block);
static uint32_t calculate_bitmap_size(MemoryInfo *mem_info);
static int calculate_num_blocks(size_t size);
static void mark_physical_memory_region_free(uintptr_t base_addr,
                                             uint32_t size);
static void mark_physical_memory_region_used(uintptr_t base_addr,
                                             uint32_t size);
static uint32_t get_num_free_blocks();
static int find_first_free_sequence(size_t num_blocks);
size_t get_used_physical_memory();
size_t get_available_physical_memory();
static int find_first_free_block();
uintptr_t get_bitmap_addr();
uint32_t get_bitmap_size();
void *alloc_block();
void free_block(void *addr);
void initialize_virtual_memory_manager();
