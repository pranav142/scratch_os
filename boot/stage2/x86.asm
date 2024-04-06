%macro x86_EnterRealMode 0
    [bits 32]
    cli
    jmp word 18h:.pmode16

.pmode16:
    [bits 16]

    mov eax, cr0
    and al, ~1 
    mov cr0, eax
    
    jmp word 00h:.rmode

.rmode:
    mov ax, 0
    mov ds, ax
    mov ss, ax
    
    sti

%endmacro


%macro x86_EnterProtectedMode 0
    cli

    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp dword 08h:.pmode

.pmode:
    [bits 32]
    
    mov ax, 0x10
    mov ds, ax
    mov ss, ax

%endmacro

