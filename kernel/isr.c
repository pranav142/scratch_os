#include "isr.h"

ISRHandler g_ISR_handlers[TOTAL_NUMBER_INTERRUPTS];

static const char *const g_exceptions[] = {"Divide by zero error",
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

void set_ISR_handler(int interrupt, ISRHandler handler) {
  g_ISR_handlers[interrupt] = handler;
  IDT_enable_gate(interrupt);
}

static void print_handler_debug_info(Registers *regs) {
  printf("Unhandled exception %d %s\n", regs->interrupt,
         g_exceptions[regs->interrupt]);
  printf("  esp=%x  ebp=%x  eip=%x  eflags=%x  cs=%x  ds=%x  ss=%x\n",
         regs->esp, regs->ebp, regs->eip, regs->eflags, regs->cs, regs->ds,
         regs->ss);
  printf("  interrupt=%x  errorcode=%x\n", regs->interrupt, regs->error);
}

void __attribute__((cdecl)) ISR_common_handler(Registers *regs) {
  if (g_ISR_handlers[regs->interrupt] != NULL) {
    g_ISR_handlers[regs->interrupt](regs);
  } else if (regs->interrupt >= TOTAL_TRAP_INTERRUPTS) {
    printf("Unhandled interrupt %d!\n", regs->interrupt);
  } else {
    print_handler_debug_info(regs);
    printf("KERNEL PANIC!\n");
    kernel_Panic();
  }
}

extern void ISR_initialize_gates();

void ISR_initialize() {
  ISR_initialize_gates();
  for (size_t i = 0; i < TOTAL_NUMBER_INTERRUPTS; i++) {
    IDT_enable_gate(i);
  }
  IDT_disable_gate(0x80);
}
