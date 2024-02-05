mov ah, 0x0e ; Specifies interrupt function (write character to screen)

mov al, 'H' ; Specifies Character to be Written
int 0x10 ; interrupt  0x10 is a BIOS ISR related to screen functions
mov al, 'e'
int 0x10
mov al, 'l'
int 0x10
mov al, 'l'
int 0x10
mov al, 'o'
int 0x10

mov al, ' '
int 0x10
mov al, 'W'
int 0x10
mov al, 'o'
int 0x10
mov al, 'r'
int 0x10
mov al, 'l'
int 0x10
mov al, 'd'
int 0x10

jmp $ ; loops forever program jumps to current instruction

times 510-($-$$) db 0
dw 0xaa55