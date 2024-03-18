#include "kernel.h"
#include "../drivers/timer.h"
#include "mem.h"

int main() {
  clear_screen();
  printf("Welcome to Scratch OS \n");

  HAL_initialize();
  printf("HAL initialized\n");
  read_memory_map();
}
