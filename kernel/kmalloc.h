#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "../drivers/screen.h"
#include "../utils/memdefs.h"
#include "paging.h"
#include "pmm.h"

typedef struct Block {
  struct Block *next;
  size_t size;
  bool free;
  uint32_t canary;
} Block;

#define INITIAL_KERNEL_HEAP_SIZE 0x10000
#define CANARY_VALUE 0x69420691
#define MIN_MEM_BLOCK_SIZE 64
#define META_BLOCK_SIZE sizeof(Block)

static Block *find_first_free_kernel_block(size_t size);
static void *ksbrk(size_t nbytes);
static Block *request_space(size_t size);
static Block *get_last_kernel_block();
static void print_heap_memory();

void initialize_kernel_heap();
void *kmalloc(size_t nbytes);
void kfree(void *ptr);

void helper_kernel_test();
void kernel_memory_test();
