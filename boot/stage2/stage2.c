#include "stage2.h"

void __attribute__((section(".text.main"))) main() {
  clear_screen();
  printf("hello");

  //  disk read functions
  //  read file system for the kernel
  //  startup the kernel
  for (;;)
    ;
  return;
}
