#include "vmm.h"

#include "kmalloc.h"
#include "paging.h"
#include "pmm.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct VirtualMemoryRegion {
  uintptr_t base;
  size_t size;
  struct VirtualMemoryRegion *next;
  bool free;
} VirtualMemoryRegion;

#define VM_META_SIZE sizeof(VirtualMemoryRegion)
#define VIRTUAL_MEMORY_START 0x400000
#define VIRTUAL_MEMORY_END 0xc0000000

#define VMM_FLAG_MMIO (1 << 3)

VirtualMemoryRegion *vm_base = NULL;

void initialize_vmm() {
  vm_base = (VirtualMemoryRegion *)kmalloc(sizeof(VirtualMemoryRegion));
  if (!vm_base) {
    return;
  }
  vm_base->base = (uintptr_t)VIRTUAL_MEMORY_START;
  vm_base->size = VIRTUAL_MEMORY_END - VIRTUAL_MEMORY_START;
  vm_base->free = true;
  vm_base->next = NULL;
}

static VirtualMemoryRegion *find_first_free_virtual_memory_region(size_t size) {
  VirtualMemoryRegion *current = vm_base;
  while (current && !(current->free && current->size >= size)) {
    current = current->next;
  }
  return current;
}

static void split_virtual_memory_region(VirtualMemoryRegion *free_region,
                                        size_t size) {
  if (free_region->size <= size) {
    return;
  }
  VirtualMemoryRegion *new_region =
      (VirtualMemoryRegion *)kmalloc(sizeof(VirtualMemoryRegion));
  if (!new_region) {
    return;
  }
  new_region->size = free_region->size - size;
  new_region->base = free_region->base + size;
  new_region->next = free_region->next;
  new_region->free = true;

  free_region->size = size;
  free_region->next = new_region;
}

static size_t align_size_to_page_size(size_t size) {
  return size = ((size + PAGE_SIZE - 1) / PAGE_SIZE) * PAGE_SIZE;
}

void *vmm_alloc(size_t size, uint32_t flags) {
  const size_t BUFFER_SIZE = 0x10;
  size = align_size_to_page_size(size);

  if (!vm_base || size <= 0) {
    return NULL;
  }

  VirtualMemoryRegion *free_region =
      find_first_free_virtual_memory_region(size);
  if (!free_region) {
    return NULL;
  }

  if (free_region->size < (size + BUFFER_SIZE)) {
    return NULL;
  }

  split_virtual_memory_region(free_region, size);

  for (uintptr_t virtual_addr = free_region->base;
       virtual_addr <= (free_region->size + free_region->base);
       virtual_addr += PAGE_SIZE) {
    void *physical_addr = alloc_block();
    if (!physical_addr) {
      return NULL;
    }

    if (!map_address(virtual_addr, (uintptr_t)physical_addr, flags)) {
      free_block(physical_addr);
    }
  }

  free_region->free = false;
  return (void *)free_region->base;
}

static VirtualMemoryRegion *find_memory_region(void *ptr) {
  VirtualMemoryRegion *current = vm_base;
  while (current) {
    if (current->base == (uintptr_t)ptr) {
      return current;
    }
    current = current->next;
  }
  return NULL;
}

void vmm_free(void *ptr) {
  if (!ptr || VIRTUAL_MEMORY_END <= (uintptr_t)ptr ||
      (uintptr_t)ptr < KERNEL_HEAP_START) {
    return;
  }

  VirtualMemoryRegion *region = find_memory_region(ptr);
  if (!region) {
    printf("failed to free memory\n");
    return;
  }

  region->free = true;

  VirtualMemoryRegion *next_region = region->next;
  if (next_region && next_region->free) {
    region->next = next_region->next;
    region->size += next_region->size;

    next_region = region->next;
  }
}

static void print_free_list() {
  VirtualMemoryRegion *current = vm_base;
  uint32_t node_number = 0;
  while (current) {
    printf("Node: %x, node base: %x, size: %x, free: %d\n", node_number,
           current->base, current->size, current->free);
    current = current->next;
    node_number++;
  }
  print("\n");
}

static size_t get_total_size() {
  VirtualMemoryRegion *current = vm_base;
  size_t total_size = 0;
  while (current) {
    total_size += current->size;
    current = current->next;
  }
  return total_size;
}

void vmm_test() {
  print_free_list();

  const size_t nbytes = 0x504;
  for (size_t i = 0; i < 100; i++) {
    void *p = vmm_alloc(nbytes, PTE_READ_WRITE);
    if (!p) {
      printf("failed allocation\n");
      return;
    }
    memset(p, 'A', nbytes);
  }
  print_free_list();
  printf("completed allocaton stress test\n");

  for (size_t i = 0; i < 100; i++) {
    void *p = vmm_alloc(nbytes, PTE_READ_WRITE);
    if (!p) {
      printf("failed allocation\n");
      return;
    }
    memset(p, 'B', nbytes);
    vmm_free(p);
  }
  print_free_list();
  printf("completed free stress test\n");

  size_t total_size = get_total_size();
  if (total_size != (VIRTUAL_MEMORY_END - VIRTUAL_MEMORY_START)) {
    printf("Error in allocations total: %x", total_size);
  }
}
