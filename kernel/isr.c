#include "isr.h"
#include "../drivers/ports.h"
#include "../drivers/screen.h"
#include "idt.h"

ISRHandler g_ISRHandlers[256];

static const char *const g_Exceptions[] = {"Divide by zero error",
                                           "Debug",
                                           "Non-maskable Interrupt",
                                           "Breakpoint",
                                           "Overflow",
                                           "Bound Range Exceeded",
                                           "Invalid Opcode",
                                           "Device Not Available",
                                           "Double Fault",
                                           "Coprocessor Segment Overrun",
                                           "Invalid TSS",
                                           "Segment Not Present",
                                           "Stack-Segment Fault",
                                           "General Protection Fault",
                                           "Page Fault",
                                           "",
                                           "x87 Floating-Point Exception",
                                           "Alignment Check",
                                           "Machine Check",
                                           "SIMD Floating-Point Exception",
                                           "Virtualization Exception",
                                           "Control Protection Exception ",
                                           "",
                                           "",
                                           "",
                                           "",
                                           "",
                                           "",
                                           "Hypervisor Injection Exception",
                                           "VMM Communication Exception",
                                           "Security Exception",
                                           ""};

void set_ISRHandler(int interrupt, ISRHandler handler) {
  g_ISRHandlers[interrupt] = handler;
  IDT_enableGate(interrupt);
}

void print_handler_debug_info(Registers *regs) {
  printf("Unhandled exception %d %s\n", regs->interrupt,
         g_Exceptions[regs->interrupt]);
  printf("  esp=%x  ebp=%x  eip=%x  eflags=%x  cs=%x  ds=%x  ss=%x\n",
         regs->esp, regs->ebp, regs->eip, regs->eflags, regs->cs, regs->ds,
         regs->ss);
  printf("  interrupt=%x  errorcode=%x\n", regs->interrupt, regs->error);
}

void __attribute__((cdecl)) ISR_Common_Handler(Registers *regs) {
  if (g_ISRHandlers[regs->interrupt] != NULL) {
    g_ISRHandlers[regs->interrupt](regs);
  } else if (regs->interrupt >= 32) {
    printf("Unhandled interrupt %d!\n", regs->interrupt);
  } else {
    print_handler_debug_info(regs);
    printf("KERNEL PANIC!\n");
    kernel_Panic();
  }
}

void ISR_InitializeGates();

void ISR_initialize() {
  ISR_InitializeGates();
  for (int i = 0; i < 256; i++) {
    IDT_enableGate(i);
  }
  IDT_disableGate(0x80);
}
