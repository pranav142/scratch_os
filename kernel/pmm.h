#pragma once

#include "../drivers/ports.h"
#include "../drivers/screen.h"
#include "../utils/memdefs.h"
#include "../utils/memtools.h"
#include "e820_mem.h"
#include <stddef.h>
#include <stdint.h>

void initialize_physical_memory_manager(MemoryInfo *mem_info,
                                        uintptr_t bitmap_addr,
                                        MemoryRegion *reserved_regions,
                                        size_t num_reserved_regions);
void *alloc_blocks(uint32_t size);
void free_blocks(void *addr, size_t size);
static inline void mark_block_free(uint32_t block);
static inline void mark_block_used(uint32_t block);
static inline bool is_block_used(uint32_t block);
static uint32_t calculate_bitmap_size(MemoryInfo *mem_info);
static inline int calculate_num_blocks(size_t size);
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
