#include "kmalloc.h"
#include "../drivers/screen.h"
#include "../utils/memdefs.h"
#include "pmm.h"
#include "vmm.h"

#define INITIAL_KERNEL_HEAP_SIZE 0x10000
#define MAX_KERNEL_HEAP_SIZE (KERNEL_HEAP_END - KERNEL_HEAP_START)

// smallest amount of memory a block can represent
#define MIN_MEM_BLOCK_SIZE 64

typedef struct {
  struct Block *next;
  size_t size;
  bool free;
} Block;

#define META_BLOCK_SIZE sizeof(Block)

static Block *g_memory_base = NULL;
static void *end_of_heap = NULL;

void initialize_kernel_heap() {
  for (uintptr_t virtual_addr = KERNEL_HEAP_START;
       virtual_addr <= (KERNEL_HEAP_START + INITIAL_KERNEL_HEAP_SIZE);
       virtual_addr += PAGE_SIZE) {
    void *physical_addr = alloc_block();
    if (!vmm_map_address(virtual_addr, (uintptr_t)physical_addr)) {
      printf("failed to map virtual address: %x to %x in kernel heap "
             "initialization",
             virtual_addr, physical_addr);
      return;
    }
  }

  g_memory_base = (Block *)KERNEL_HEAP_START;
  g_memory_base->free = true;
  g_memory_base->size = INITIAL_KERNEL_HEAP_SIZE;
  g_memory_base->next = NULL;

  end_of_heap = (void *)((uintptr_t)g_memory_base + g_memory_base->size);
  printf("end of heap: %x\n", end_of_heap);
}

Block *find_first_free_kernel_block(size_t size) {
  Block *current = g_memory_base;
  while (current && !(current->free && current->size >= size)) {
    current = (Block *)current->next;
  }
  return current;
}

void *ksbrk(size_t nbytes) {
  if (((uintptr_t)end_of_heap + nbytes) > KERNEL_HEAP_END)
    return NULL;

  void *prev_heap_end = end_of_heap;
  void *new_heap_end = (void *)((uintptr_t)end_of_heap + nbytes);

  // map new heap region
  for (uintptr_t p = (uintptr_t)prev_heap_end; p <= (uintptr_t)new_heap_end;
       p += PAGE_SIZE) {
    void *physical_addr = alloc_block();
    if (!physical_addr) {
      return NULL;
    }

    if (!vmm_map_address(p, (uintptr_t)physical_addr)) {
      free_block(physical_addr);
    }
  }

  end_of_heap = new_heap_end;
  return prev_heap_end;
}

Block *request_space(size_t size) {
  Block *block;
  block = (Block *)ksbrk(0);
  void *request = ksbrk(size + META_BLOCK_SIZE);
  if (block != request) {
    printf("There was an error with ksbrk\n");
    return NULL;
  }

  if (!request) {
    return NULL;
  }

  block->size = size;
  block->next = NULL;
  block->free = false;
  return block;
}

void *kmalloc(size_t nbytes) {
  if (nbytes <= 0 || !g_memory_base) {
    return NULL;
  }

  Block *block = find_first_free_kernel_block(nbytes);
  if (!block) {
    block = request_space(nbytes);
    if (!block) {
      return NULL;
    }
  } else if (block->size > (nbytes + META_BLOCK_SIZE + MIN_MEM_BLOCK_SIZE)) {
    size_t original_size = block->size;
    block->size = nbytes;
    Block *new_block =
        (Block *)((uintptr_t *)block + block->size + META_BLOCK_SIZE);

    new_block->free = true;
    new_block->next = block->next;
    new_block->size = original_size - nbytes - META_BLOCK_SIZE;

    block->next = (struct Block *)new_block;
  }

  block->free = false;
  return (void *)((uintptr_t)block + META_BLOCK_SIZE);
}

void kfree(void *ptr) {
  if (!ptr) {
    return;
  }

  Block *block = (Block *)((uintptr_t)ptr - META_BLOCK_SIZE);
  block->free = true;
  // add coallescing blocks
}

void kernel_memory_test() {
  Block *p = find_first_free_kernel_block(100);
  if ((uintptr_t)p == KERNEL_HEAP_START) {
    printf("success finding kernel block\n");
  }

  uint32_t nbytes = 0x1000;
  void *x = kmalloc(nbytes);
  printf("size of metadata: %x\n", META_BLOCK_SIZE);
  printf("%x\n", x);

  // this currently causes a page fault
  //  x = kmalloc(50000);

  Block *current = g_memory_base;
  uint32_t count = 1;
  while (current) {
    printf("Node: %d, size: %x, free: %d\n", count, current->size,
           current->free);
    current = current->next;
    count++;
  }
  // uintptr_t prev_end_of_heap = (uintptr_t)end_of_heap;
  // p = request_space(nbytes);
  // if ((uintptr_t)p == prev_end_of_heap && p->size == nbytes) {
  //   printf("success requesting space\n");
  // }

  // void *x = ksbrk(nbytes);
  // // printf("ksbrk return value %x, end of heap: %x\n", x,
  // end_of_heap); if ((uintptr_t)end_of_heap == ((uintptr_t)x +
  // nbytes)) {
  //   printf("success using ksbrk\n");
  // }

  // x = ksbrk(nbytes);
  // // printf("ksbrk return value %x, end of heap: %x\n", x,
  // end_of_heap); if ((uintptr_t)end_of_heap == ((uintptr_t)x +
  // nbytes)) {
  //   printf("success using ksbrk\n");
  // }
}
