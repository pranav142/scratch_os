mov al, 'H'
call print_function

jmp $

print_function:
    pusha
    mov ah, 0x0e
    int 0x10
    popa 
    ret

times 510-($-$$) db 0
dw 0xaa55