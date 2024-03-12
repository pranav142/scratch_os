#include "ports.h"

unsigned char port_byte_in(unsigned short port) {
    unsigned char result;
    __asm__("in %%dx, %%al" : "=a"(result) : "d"(port));
    return result;
}

void port_byte_out(unsigned short port, unsigned char data) {
    __asm__("out %%al, %%dx" : : "a"(data), "d"(port));
}

unsigned short port_word_in(unsigned short port) {
    unsigned short result;
    __asm__("in %%dx, %%ax" : "=a"(result) : "d"(port));
    return result;
}

void port_word_out(unsigned short port, unsigned short data) {
    __asm__("out %%ax, %%dx" : : "a"(data), "d"(port));
}

void kernel_Panic() {
    // disables interrupts, halts CPU
    __asm__("cli\n\t"  
            "hlt");   
}

void io_wait() { 
    // volatile prevents compiler optimization
    __asm__ volatile ("outb %%al, $0x80" : : "a"(0));
}

void enable_interrupts() { 
    __asm__ ("sti");
}

void disable_interrupts() { 
    __asm__ ("cli");
}
