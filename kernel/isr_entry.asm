[bits 32]

extern ISR_Common_Handler

%macro ISR_NOERRORCODE 1

global ISR%1
ISR%1:
  push 0
  push %1
  jmp isr_common

%endmacro

%macro ISR_ERRORCODE 1

global ISR%1
ISR%1:
  push %1
  jmp isr_common

%endmacro

%include "kernel/isr.inc"

isr_common:
  pusha

  xor eax, eax ; clearing eax
  mov ax, ds   ; saving the initial data segment selector in ax 
  push eax     ; pushing the initial segment selector onto the stack

  mov ax, 0x10 ; this points to the data segment
  mov ds, ax   ; all data segments now point to the kernel data segment
  mov es, ax
  mov fs, ax
  mov gs, ax
  
  push esp            ; pass pointer to stack to C, so we can access all the pushed information
  call ISR_Common_Handler    ; ISR handler can use pointer stack esp to get values on the stack and use that info to handler values
  add esp, 4          ; this basically pushes the stack back up so the pointer to stack is removed from stack

  pop eax             ; restore old segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  popa                ; pop what we pushed with pusha
  add esp, 8          ; remove error code and interrupt number
  iret   
