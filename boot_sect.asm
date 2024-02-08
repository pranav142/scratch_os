org 0x7c00

mov bx, HELLO_MSG ; bx points to the first character address in HELLO_MSG
call print_string

mov dx, HELLO_MSG
call print_hex

jmp $ ; loops forever program jumps to current instruction

; make sure to put this below the jmp $ otherwise at the end of the program this will prematurely 
; be run despite never being called
%include "print_string.asm"
%include "print_hex.asm"

HELLO_MSG:
    db 'hello, world',0

times 510-($-$$) db 0
dw 0xaa55