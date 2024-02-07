mov ah, 0x0e

mov bx, 30

cmp bx, 4
jle print_A ; if bx <= 4 print A
cmp bx, 40
jl print_B ; else if bx < 40 print B
jmp print_C ; else print C 

int 0x10

print_A:
    mov al, 'A'
    jmp print_char

print_B:
    mov al, 'B'
    jmp print_char

print_C:
    mov al, 'C'
    jmp print_char

print_char:
    int 0x10
    jmp $

times 510-($-$$) db 0
dw 0xaa55