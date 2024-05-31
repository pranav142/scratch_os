#include "idt.h"

IDTEntry g_IDT[TOTAL_NUMBER_INTERRUPTS];

IDTDescriptor g_IDT_descriptor = {sizeof(g_IDT) - 1, g_IDT};

extern void __attribute__((cdecl)) IDT_Load(IDTDescriptor *idtDescriptor);

void IDT_set_gate(int interrupt, void *ISR_addr, uint16_t segment_selector,
                  uint8_t flags) {
  g_IDT[interrupt].offset_low = ((uintptr_t)ISR_addr) & 0xFFFF;
  g_IDT[interrupt].segment_selector = segment_selector;
  g_IDT[interrupt].unused = 0;
  g_IDT[interrupt].flags = flags;
  g_IDT[interrupt].offset_high = (((uintptr_t)ISR_addr) >> 16) & 0xFFFF;
}

void IDT_enable_gate(int interrupt) {
  g_IDT[interrupt].flags = g_IDT[interrupt].flags | IDT_FLAG_PRESENT;
}

void IDT_disable_gate(int interrupt) {
  g_IDT[interrupt].flags = g_IDT[interrupt].flags & ~IDT_FLAG_PRESENT;
}

void IDT_initialize() { IDT_Load(&g_IDT_descriptor); }
