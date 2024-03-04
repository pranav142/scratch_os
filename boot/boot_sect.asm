[org 0x7c00]

; Set the stack.
mov bp , 0x9000 
mov sp , bp

mov bx , MSG_REAL_MODE
call print_string

call switch_to_pm

jmp $

%include "boot/print_string.asm"
%include "boot/32_bit_print.asm"
%include "boot/print_hex.asm"
%include "boot/disk_load.asm"
%include "boot/GDT.asm"
%include "boot/switch_to_pm.asm"


BEGIN_PM:
    mov ebx , MSG_32_MODE
    ; call clear_screen_pm
    call print_string_pm ; Use our 32 - bit print routine.
    jmp $

MSG_REAL_MODE: db "Starting in 16 bit real Mode", 0
MSG_32_MODE: db "Now in 32 bit protected Mode", 0


; we are the first sector
times 510-($-$$) db 0
dw 0xaa55
