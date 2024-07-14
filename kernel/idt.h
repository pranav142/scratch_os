#pragma once
#include <stdint.h>

typedef enum {
  IDT_FLAG_GATE_TASK = 0x5,
  IDT_FLAG_GATE_16_INT = 0x6,
  IDT_FLAG_GATE_16_TRAP = 0x7,
  IDT_FLAG_GATE_32_INT = 0xE,
  IDT_FLAG_GATE_32_TRAP = 0xF,

  IDT_FLAG_RING_0 = (0 << 5),
  IDT_FLAG_RING_1 = (1 << 5),
  IDT_FLAG_RING_2 = (2 << 5),
  IDT_FLAG_RING_3 = (3 << 5),

  IDT_FLAG_PRESENT = 0x80,
} IDTFlags;

typedef struct {
  uint16_t offset_low;
  uint16_t segment_selector;
  uint8_t unused;
  uint8_t flags;
  uint16_t offset_high;
} __attribute__((packed)) IDTEntry;

typedef struct {
  uint16_t limit;
  IDTEntry *ptr;
} __attribute__((packed)) IDTDescriptor;

#define TOTAL_NUMBER_INTERRUPTS 256
#define CODE_SEGMENT 0x08
#define DATA_SEGMENT 0x10

void IDT_initialize();
void IDT_disable_gate(int interrupt);
void IDT_enable_gate(int interrupt);
void IDT_set_gate(int interrupt, void *ISR_addr, uint16_t segment_selector,
                  uint8_t flags);
