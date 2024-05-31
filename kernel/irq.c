#include "irq.h"

ISRHandler g_IRQ_handlers[TOTAL_HARDWARE_INTERRUPTS];

void set_IRQ_handler(int IRQ_line, ISRHandler handler) {
  g_IRQ_handlers[IRQ_line] = handler;
}

void IRQ_common_handler(Registers *regs) {
  int irq = regs->interrupt - PIC1_OFFSET;
  if (g_IRQ_handlers[irq] != NULL) {
    g_IRQ_handlers[irq](regs);
  } else {
    printf("Unhandled IRQ %d\n", irq);
  }

  PIC_sendEOI(irq);
}

void IRQ_initialize() {
  PIC_remap(PIC1_OFFSET, PIC2_OFFSET);

  for (size_t i = 0; i < TOTAL_HARDWARE_INTERRUPTS; i++) {
    set_ISR_handler(PIC1_OFFSET + i, IRQ_common_handler);
  }

  enable_interrupts();
}
