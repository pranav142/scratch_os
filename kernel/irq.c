#include "irq.h"

ISRHandler g_IRQHandlers[16];

void set_IRQHandler(int IRQline, ISRHandler handler) {
  g_IRQHandlers[IRQline] = handler;
}

void IRQ_Common_Handler(Registers *regs) {
  int irq = regs->interrupt - PIC1_OFFSET;
  if (g_IRQHandlers[irq] != NULL) {
    g_IRQHandlers[irq](regs);
  } else {
    printf("Unhandled IRQ %d\n", irq);
  }

  PIC_sendEOI(irq);
}

void IRQ_initialize() {
  PIC_remap(PIC1_OFFSET, PIC2_OFFSET);

  for (int i = 0; i < 16; i++) {
    set_ISRHandler(PIC1_OFFSET + i, IRQ_Common_Handler);
  }

  enable_interrupts();
}
