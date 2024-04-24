#include "kernel.h"
#include "mem.h"

extern uint8_t __entry_start;
extern uint8_t __end;

void __attribute__((section(".entry"))) start(MemoryInfo mem_info) {
  MemoryRegion reserved_regions[] = {
      {.start_address = (uintptr_t)&__entry_start,
       .length = (size_t)(&__end - &__entry_start)},
      {.start_address = (uintptr_t)0x0, .length = (size_t)0x9fc00},
  };

  size_t num_reserved_regions =
      sizeof(reserved_regions) / sizeof(reserved_regions[0]);

  clear_screen();

  printf("Welcome to Scratch OS \n");

  HAL_initialize();
  print_memory_map(&mem_info);
  initialize_physical_memory_manager(&mem_info, (uintptr_t)&__end,
                                     reserved_regions, num_reserved_regions);

  for (;;)
    ;
}
