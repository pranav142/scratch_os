#pragma once

unsigned char port_byte_in(unsigned short port);
void port_byte_out(unsigned short port, unsigned char data);
unsigned short port_word_in(unsigned short port);
void port_word_out(unsigned short port, unsigned short data);
void __attribute__((cdecl)) kernel_Panic();
void io_wait();
void enable_interrupts();
void disable_interrupts();
