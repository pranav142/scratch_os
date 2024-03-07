#include "../drivers/screen.h"
#include "../drivers/ports.h"
#include "idt.h"

void main() {
    IDT_initialize();

    clear_screen();
    print("Initalized IDT\n");
    print("Hello World!");
}


