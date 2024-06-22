#include "kmalloc.h"

static Block *g_memory_base = NULL;
static void *end_of_heap = NULL;

void initialize_kernel_heap() {
  for (uintptr_t virtual_addr = KERNEL_HEAP_START;
       virtual_addr <= (KERNEL_HEAP_START + INITIAL_KERNEL_HEAP_SIZE);
       virtual_addr += PAGE_SIZE) {
    void *physical_addr = alloc_block();
    if (!map_address(virtual_addr, (uintptr_t)physical_addr, PTE_READ_WRITE)) {
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
  g_memory_base->canary = CANARY_VALUE;

  end_of_heap = (void *)((uintptr_t)g_memory_base + g_memory_base->size);
}

static Block *find_first_free_kernel_block(size_t size) {
  Block *current = g_memory_base;
  while (current && !(current->free && current->size >= size)) {
    current = (Block *)current->next;
  }
  return current;
}

// TODO: come up with faster method for mapping
static void *ksbrk(size_t nbytes) {
  if (((uintptr_t)end_of_heap + nbytes) > KERNEL_HEAP_END)
    return NULL;

  void *prev_heap_end = end_of_heap;
  void *new_heap_end = (void *)((uintptr_t)end_of_heap + nbytes);

  for (uintptr_t virtual_addr = (uintptr_t)prev_heap_end;
       virtual_addr <= (uintptr_t)new_heap_end; virtual_addr++) {
    void *physical_addr = alloc_block();
    if (!physical_addr) {
      return NULL;
    }

    if (!map_address(virtual_addr, (uintptr_t)physical_addr, PTE_READ_WRITE)) {
      free_block(physical_addr);
    }
  }

  end_of_heap = new_heap_end;
  return prev_heap_end;
}

static Block *request_space(size_t size) {
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
  block->canary = CANARY_VALUE;
  return block;
}

static Block *get_last_kernel_block() {
  Block *current = g_memory_base;
  while (current->next) {
    current = current->next;
  }
  return current;
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

    Block *last_block = get_last_kernel_block();
    last_block->next = block;
  } else if (block->size > (nbytes + META_BLOCK_SIZE + MIN_MEM_BLOCK_SIZE)) {
    size_t original_size = block->size;
    block->size = nbytes;
    Block *new_block =
        (Block *)((uintptr_t)block + block->size + META_BLOCK_SIZE);

    new_block->free = true;
    new_block->next = block->next;
    new_block->size = original_size - nbytes - META_BLOCK_SIZE;
    new_block->canary = CANARY_VALUE;

    block->next = (Block *)new_block;
  }

  block->free = false;
  return (void *)((uintptr_t)block + META_BLOCK_SIZE);
}

static inline bool is_valid_block(Block *block) {
  return block->canary == CANARY_VALUE;
}

// TODO: fix a bug wih freeing memory
void kfree(void *ptr) {
  if (!ptr || KERNEL_HEAP_END <= (uintptr_t)ptr ||
      (uintptr_t)ptr <= KERNEL_HEAP_START) {
    return;
  }

  Block *block = (Block *)((uintptr_t)ptr - META_BLOCK_SIZE);
  if (!is_valid_block(block)) {
    printf("Invalid Free Pointer or Heap Memory Corruption!\n");
    kernel_Panic();
    return;
  }

  block->free = true;

  Block *next_block = block->next;
  if (next_block && next_block->free) {
    block->next = next_block->next;
    block->size += next_block->size + META_BLOCK_SIZE;
  }
}

static void print_heap_memory() {
  Block *current = g_memory_base;
  uint32_t count = 1;
  while (current) {
    printf("Node: %d, size: %x, free: %d\n", count, current->size,
           current->free);
    current = current->next;
    count++;
  }
  printf("\n");
}

void helper_kernel_test() {
  uint32_t nbytes = 0x1000;
  Block *p = find_first_free_kernel_block(100);
  if ((uintptr_t)p == KERNEL_HEAP_START) {
    printf("success finding kernel block\n");
  }
  uintptr_t prev_end_of_heap = (uintptr_t)end_of_heap;
  p = request_space(nbytes);
  if ((uintptr_t)p == prev_end_of_heap && p->size == nbytes) {
    printf("success requesting space\n");
  }

  void *x = ksbrk(nbytes);
  printf("ksbrk return value %x, end of heap: %x\n", x, end_of_heap);
  if ((uintptr_t)end_of_heap == ((uintptr_t)x + nbytes)) {
    printf("success using ksbrk\n");
  }

  x = ksbrk(nbytes);
  printf("ksbrk return value %x, end of heap: %x\n", x, end_of_heap);
  if ((uintptr_t)end_of_heap == ((uintptr_t)x + nbytes)) {
    printf("success using ksbrk\n");
  }
}

void kernel_memory_test() {
  uint32_t nbytes = 0x1000;
  void *p1 = kmalloc(nbytes);
  printf("size of metadata: %x\n", META_BLOCK_SIZE);
  printf("%x\n", p1);

  void *p2 = kmalloc(0x50000);
  void *p3 = kmalloc(0x50000);
  void *p4 = kmalloc(0x50000);
  kfree(p4);
  kfree(p1);
  print_heap_memory();

  void *x = kmalloc(0x50000);
  if (x != NULL) {
    memset(x, 'A', 0x50000);
    for (size_t i = 0x45000; i < 0x45010; i++) {
      printf("%c", ((char *)x)[i]);
    }
    printf("Success: setting allocated memory large\n");
  }
  kfree(x);

  x = kmalloc(0x500);
  if (x != NULL) {
    memset(x, 'b', 0x500);
    for (size_t i = 0x450; i < 0x460; i++) {
      printf("%c", ((char *)x)[i]);
    }
    printf("success: setting allocated memory small\n");
  }
  print_heap_memory();

  kfree(x);
  kfree(p3);
  kfree(p2);
  print_heap_memory();

  for (size_t i = 0; i < 100; i++) {
    void *ptr = kmalloc(0x5000 + i * 5);
    memset(ptr, 'A' + i, 0x500);
    if (!ptr) {
      printf("failed\n");
      return;
    }
    kfree(ptr);
  }

  print_heap_memory();
  printf("success: stress test\n");

  void *ptr;
  for (size_t i = 0; i < 500; i++) {
    ptr = kmalloc(0x100000);
    memset(ptr, 'A' + i, 0x500);
  }
  if (!ptr) {
    printf("success: allocating all memory\n");
  } else {
    printf("Failed allocating all memory");
    return;
  }
}
