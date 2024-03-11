[bits 32]

global IDT_Load
IDT_Load:
    push ebp
    mov ebp, esp

    mov eax, [esp + 8]
    lidt [eax]

    mov esp, ebp
    pop ebp
    ret
     
