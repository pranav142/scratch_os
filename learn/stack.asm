; a program to understand stack operations

mov ah, 0x0e

mov bp, 0x8000 ; setting the base of the stack at 0x8000 which is far from the boot sector (0x7c00)
mov sp, bp ; stack is empty so top and bottom of stack are at same address

; x86 is fundamentally a 16 bit arhitecture; stack oeprations are 16bits wide. 
; push adds 16 bits (2bytes) to the stack ; pop removes 16 bits (2 bytes) to the stack
push 'A' ; each of these operations pushes sp down and occupies 16 bits despite being 8 bits long
push 'B' 
push 'C' ; C is at the top of the stack

pop bx ; Stores top of stack in BX a 16 bit register composed of BH (high byte) and BL (low byte) 
       ; first 8 bits are filled with 0 during push operation last 8 contain the character
mov al, bl
int 0x10

mov al, [0x7FFE] ; 0X8000 - 2 bytes is 0x7FFE thus this prints one
int 0x10

jmp $

times 510-($-$$) db 0
dw 0xaa55