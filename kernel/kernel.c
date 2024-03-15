#include "kernel.h"
#include "../drivers/timer.h"

void main() {
    clear_screen();
    printf("Welcome to Scratch OS \n");

    HAL_initialize();
    printf("HAL initialized\n");
    
    printf("sleeping for 5 seconds");
    sleep(5000);
    printf("done sleeping");
}


