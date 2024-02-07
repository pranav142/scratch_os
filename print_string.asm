print_string:
    pusha ; push all previous register values to the stack

    loop:
        cmp byte [bx], 0 ; byte comparison of bx to 0 (string temrinator)
        je the_end ; if we reach terminator jump to end 
        mov al, [bx] ; else print the character
        call print_char
        inc bx ; inc the address of bx so we can see next character
        jmp loop ; loop until we reach string terminator
    the_end:
        popa ; pops all register values on stack back to registers
        ret ; pops prorgam execution address from stack and jumps to it
    
print_char:
    pusha
    mov ah, 0x0e
    int 0x10
    popa
    ret

