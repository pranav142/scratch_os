; a program to understand memory management in a bare non-os enviornment

[org 0x7c00] ; this tells the assembler where our code is loaded into memory so it can handle
             ; relative addressing, Ignore this for the situations below

mov ah, 0x0e

mov al, the_secret ; treats the_secret as a value and moves the data into the register but the secret is a address
int 0x10

mov al, [the_secret] ; treats the_secret as a address and moves the value at 0x1e but value is really at 0x7c1e
int 0x10

mov bx, the_secret ; move the offset for x 
add bx, 0x7c00 ; add the offset to the start of bootsector code
mov al, [bx] ; gets the actual value at the correctly adjusted bootsector
int 0x10

mov al, [0x7c1e] ; we add 0x7c00 ti the offset where x is at (0x1e)
int 0x10

jmp $

the_secret: ; the secret holds the address to where X is written 
    db "X" ; by looking at the compiled code we know it is offset 30(0x1e) bytes

times 510-($-$$) db 0
dw 0xaa55