#include "kernel.h"
#include "kmalloc.h"

extern uint8_t __end;

void __attribute__((section(".entry"))) start(MemoryInfo mem_info) {
  MemoryRegion reserved_regions[] = {
      {.start_address = 0x0, .length = 0x400000}};

  size_t num_reserved_regions =
      sizeof(reserved_regions) / sizeof(reserved_regions[0]);

  clear_screen();

  printf("Welcome to Scratch OS \n");

  initialize_physical_memory_manager(&mem_info, (uintptr_t)&__end,
                                     reserved_regions, num_reserved_regions);
  initialize_virtual_memory_manager();

  HAL_initialize();

  print_memory_map(&mem_info);

  // memory_test();
  initialize_kernel_heap();

  kernel_memory_test();

  for (;;)
    ;
}
