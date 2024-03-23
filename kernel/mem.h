#pragma once

#include "../drivers/screen.h"
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

typedef struct {
  uint32_t present : 1;       // Page present in memory
  uint32_t rw : 1;            // Read-only if clear, readwrite if set
  uint32_t user : 1;          // Supervisor level only if clear
  uint32_t writeThrough : 1;  // Write-through caching if set
  uint32_t cacheDisabled : 1; // Cache disabled if set
  uint32_t accessed : 1;      // Has the page been accessed since last refresh?
  uint32_t dirty : 1;     // Has the page been written to since last refresh?
  uint32_t pat : 1;       // Page Attribute Table
  uint32_t global : 1;    // Global page if set
  uint32_t available : 3; // Available for system programmer use
  uint32_t physical_memoty_block : 20;
} __attribute__((packed)) PageTableEntry;

typedef struct {
  uint32_t present : 1;       // Page present in memory
  uint32_t rw : 1;            // Read-only if clear, readwrite if set
  uint32_t user : 1;          // Supervisor level only if clear
  uint32_t writeThrough : 1;  // Write-through caching if set
  uint32_t cacheDisabled : 1; // Cache disabled if set
  uint32_t accessed : 1;      // Has the page been accessed since last refresh?
  uint32_t dirty : 1;     // Has the page been written to since last refresh?
  uint32_t pat : 1;       // Page Attribute Table
  uint32_t available : 4; // Available for system programmer use
  uint32_t physical_memoty_block : 20;
} __attribute__((packed)) PageDirectoryEntry;

#define MEMORY_MAP_COUNT_ADDR 0x8000
#define MEMORY_MAP_ADDR 0x8004
#define MAX_ENTRIES 16

#define BITMAP_BLOCK_SIZE 4096
#define MEMORY_START_ADDR 0x100000
#define TOTAL_USABLE_MEMORY 0x7ee0000
#define BITMAP_BLOCK_COUNT TOTAL_USABLE_MEMORY / BITMAP_BLOCK_SIZE
#define BLOCKS_PER_INDEX 16
#define BITMAP_SIZE BITMAP_BLOCK_COUNT / BLOCKS_PER_INDEX

#define BLOCK_TO_ADDR(b) (b * BITMAP_BLOCK_SIZE) + MEMORY_START_ADDR
#define ADDR_TO_BLOCK(a) (a - MEMORY_START_ADDR) / BITMAP_BLOCK_SIZE

#define MAX_PAGE_ENTRIES 1024

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
