#include "irq.h"

void PIC_sendEOI(uint8_t irq) {
    if(irq >= 8) {
        port_byte_out(PIC2_COMMAND, PIC_EOI);
    }

    port_byte_out(PIC1_COMMAND, PIC_EOI);
}

/*
 *
 * offset1 - irq + offset1 = interrupt
 * offset2 - irq + offset2 = interrupt
 * offset1 + 7 and offset 2 + 7 show the span of interrupt nums to CPU
*/
void PIC_remap(int offset1, int offset2) {
	port_byte_out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  	// begin initialization
	io_wait();
	port_byte_out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
	io_wait();
	port_byte_out(PIC1_DATA, offset1);                 		// initialize offsets
	io_wait();
	port_byte_out(PIC2_DATA, offset2);                 
	io_wait();
	port_byte_out(PIC1_DATA, 4);                        	// tell master slave is connected to IRQ 2
	io_wait();
	port_byte_out(PIC2_DATA, 2);                           
	io_wait();
 
	port_byte_out(PIC1_DATA, ICW4_8086);                	// sets operation mode
	io_wait();
	port_byte_out(PIC2_DATA, ICW4_8086);
	io_wait();
 
	port_byte_out(PIC1_DATA, 0);   						// restore masks
	port_byte_out(PIC2_DATA, 0);	
}

void IRQ_set_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;

    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = port_byte_in(port) | (1 << IRQline);
    port_byte_out(port, value);        
}
 
void IRQ_clear_mask(uint8_t IRQline) {
    uint16_t port;
    uint8_t value;
 
    if(IRQline < 8) {
        port = PIC1_DATA;
    } else {
        port = PIC2_DATA;
        IRQline -= 8;
    }
    value = port_byte_in(port) & ~(1 << IRQline);
    port_byte_out(port, value);        
}

void initialize_PIC() {
    PIC_remap(0x20, 0x28);
    enable_interrupts();
}
