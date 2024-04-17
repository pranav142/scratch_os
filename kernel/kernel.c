#include "kernel.h"
#include "mem.h"

void __attribute__((section(".entry"))) main() {
  clear_screen();
  printf("Welcome to Scratch OS \n");

  // HAL_initialize();
  // printf("HAL initialized\n");
  // read_memory_map();
  // print_memory_map();
  // initialize_physical_bit_map();
  // for (int i = 0; i < 100; i++) {
  // void *mem = allocate_block();
  // free_block(mem);
  //}
  // print_physical_bit_map(0, 10);

  for (;;)
    ;
}
