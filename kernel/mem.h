#pragma once

#include "../drivers/ports.h"
#include "../drivers/screen.h"
#include "../utils/memdefs.h"
#include "../utils/memtools.h"
#include "isr.h"
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

#define SELF_MAP_BASE 0xFFC00000

#define PD_INDEX(addr) ((addr >> 22) & 0x3FF)
#define PT_INDEX(addr) ((addr >> 12) & 0x3FF)
#define PT_OFFSET(addr) (addr & 0xFFF)
#define SET_FLAG(entry, flag) (*entry |= flag)
#define CLEAR_FLAG(entry, flag) (*entry &= ~flag)
#define TEST_FLAG(entry, flag) (*entry & flag)
#define SET_ADDR(entry, addr)                                                  \
  (*(entry) = (*(entry) & ~0xFFFFF000) | ((addr) & 0xFFFFF000))
#define GET_ADDR(entry) (*entry & 0xFFFFF000)
#define PT_VIRTUAL_ADDR(page_dir_index)                                        \
  (SELF_MAP_BASE + (page_dir_index * 0x1000))

#define TABLES_PER_DIRECTORY 1024
#define PAGES_PER_TABLE 1024

#define PAGE_FAULT_INTERRUPT_NUMBER 0xE

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

typedef enum {
  PAGE_PROTECTION_VIOLATION = 0x01, // not set -> not present page
  INVALID_WRITE_ACCESS = 0x02,      // not set -> invalid read access
  PRIVILIGE_VIOLATION = 0x4,
  RESERVED_BIT_VIOLATION = 0x8,
  INSTRUCTION_FETCH_VIOLATION = 0x10,
  PROTECTION_KEY_VIOLATION = 0x20,
  SHADOW_STACK_ACCESS = 0x30,
  SOFTWARE_GUARD_VIOLATION = 0x40,
} PAGE_FAULT_ERRORS;

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
bool vmm_map_page(uintptr_t virtual_addr, uintptr_t physical_addr);
void vmm_unmap_page(uintptr_t virtual_address);
void vmm_free_page(uintptr_t virtual_address);
static void enable_paging();
static bool is_paging();
void memory_test();
