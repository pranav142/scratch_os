#include "../drivers/screen.h"
#include "../drivers/ports.h"
#include "idt.h"

void main() {
    IDT_initialize();

    clear_screen();
    print("Initalized IDT\n");
    print("Hello World!\n");
    printf("Formatted String num monkeys %d press %c to quit\n", 20, 'q');
}


