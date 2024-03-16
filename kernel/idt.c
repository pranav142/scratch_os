#include "idt.h"

typedef struct {
  uint16_t OffsetLow;
  uint16_t SegmentSelector;
  uint8_t Unused;
  uint8_t Flags;
  uint16_t OffsetHigh;
} __attribute__((packed)) IDTEntry;

typedef struct {
  uint16_t Limit;
  IDTEntry *Ptr;
} __attribute__((packed)) IDTDescriptor;

IDTEntry g_IDT[256];

IDTDescriptor g_IDTDescriptor = {sizeof(g_IDT) - 1, g_IDT};

void __attribute__((cdecl)) IDT_Load(IDTDescriptor *idtDescriptor);

void IDT_SetGate(int interrupt, void *ISR_Addr, uint16_t segment_selector,
                 uint8_t flags) {
  g_IDT[interrupt].OffsetLow = ((uint32_t)ISR_Addr) & 0xFFFF;
  g_IDT[interrupt].SegmentSelector = segment_selector;
  g_IDT[interrupt].Unused = 0;
  g_IDT[interrupt].Flags = flags;
  g_IDT[interrupt].OffsetHigh = (((uint32_t)ISR_Addr) >> 16) & 0xFFFF;
}

void IDT_enableGate(int interrupt) {
  // sets present flag to 1
  g_IDT[interrupt].Flags = g_IDT[interrupt].Flags | IDT_FLAG_PRESENT;
}

void IDT_disableGate(int interrupt) {
  // Sets present flag to 0
  g_IDT[interrupt].Flags = g_IDT[interrupt].Flags & ~IDT_FLAG_PRESENT;
}

void IDT_initialize() { IDT_Load(&g_IDTDescriptor); }
