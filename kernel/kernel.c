#include "kernel.h"
#include "../drivers/timer.h"

void main() {
    clear_screen();
    printf("Welcome to Scratch OS \n");

    HAL_initialize();
    printf("HAL initialized\n");
    
    printf("sleeping for 5 seconds\n");
    uint64_t start = time_now();
    sleep(5000);
    uint64_t end = time_now();
    printf("done sleeping %d\n", end - start);
}


