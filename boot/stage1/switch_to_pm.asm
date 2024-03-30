[bits 16]

; switching from 16 bit real mode to 32 bit protected
switch_to_pm:
    cli        ; disables interrupts
    
    lgdt [gdt_descriptor]

    mov eax , cr0 ; make the first bit of control register 1
    or eax , 0x1 
    mov cr0, eax

    jmp CODE_SEG:init_pm

[bits 32]

init_pm:

    mov ax , DATA_SEG   ; start of data segment

    ; all data sectors point to data segment
    mov ds , ax 
    mov ss , ax 
    mov es , ax
    mov fs , ax
    mov gs , ax

    ; update stack position
    mov ebp , 0x90000 
    mov esp , ebp 

    ; replace with a call to something
    call BEGIN_PM
