[org 0x7c00]
mov ah, 0x0e ; Specifies interrupt function (write character to screen)

mov al, [message]
int 0x10

jmp $ ; loops forever program jumps to current instruction

message:
    db "Hello World", 0 

times 510-($-$$) db 0
dw 0xaa55