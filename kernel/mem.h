#pragma once

#include "../drivers/screen.h"
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

#define MEMORY_MAP_COUNT_ADDR 0x500
#define MEMORY_MAP_ADDR 0x504
#define MAX_ENTRIES 16

#define BITMAP_BLOCK_SIZE 4096
#define MEMORY_START_ADDR 0x100000
#define TOTAL_USABLE_MEMORY 0x7ee0000
#define BITMAP_BLOCK_COUNT TOTAL_USABLE_MEMORY / BITMAP_BLOCK_SIZE
#define BLOCKS_PER_INDEX 16
#define BITMAP_SIZE BITMAP_BLOCK_COUNT / BLOCKS_PER_INDEX

#define BLOCK_TO_ADDR(b) (b * BITMAP_BLOCK_SIZE) + MEMORY_START_ADDR
#define ADDR_TO_BLOCK(a) (a - MEMORY_START_ADDR) / BITMAP_BLOCK_SIZE

void read_memory_map();
void print_memory_map();
void initialize_physical_bit_map();
void mark_physical_block_used(int block);
void print_physical_bit_map(int begin, int end);
void mark_physical_block_unused(int block);
int find_free_block();
bool block_is_used(int block);
void *allocate_block();
void free_block(void *mem_addr);
