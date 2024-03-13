#include "kernel.h"

void main() {
    clear_screen();
    printf("Welcome to Scratch OS \n");

    HAL_initialize();
    printf("HAL initialized\n");
}


