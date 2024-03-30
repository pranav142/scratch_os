[org 0x7c00]

mov [ BOOT_DRIVE ], dl 
STAGE2_OFFSET equ 0x1000

; Set the stack.
mov bp , 0x9000 
mov sp , bp

mov bx , MSG_REAL_MODE
call print_string

call load_stage2

call do_e820

call switch_to_pm

jmp $

%include "print_string.asm"
%include "32_bit_print.asm"
%include "print_hex.asm"
%include "disk_load.asm"
%include "GDT.asm"
%include "switch_to_pm.asm"
%include "memdetect.asm"

[bits 16]
load_stage2:
    mov bx , STAGE2_LOAD_MSG
    call print_string

    mov bx, STAGE2_OFFSET ; Data will be stored at the offset
    mov dh, 50
    mov dl, [BOOT_DRIVE]
    call disk_load  

    ret

[bits 32]
BEGIN_PM:
    mov ebx , MSG_32_MODE
    call print_string_pm

    call clear_screen_pm

    call STAGE2_OFFSET

    jmp $

BOOT_DRIVE: db 0
MSG_REAL_MODE: db "real", 0
MSG_32_MODE: db "32", 0
STAGE2_LOAD_MSG: db "kernel", 0


; we are the first sector
times 510-($-$$) db 0
dw 0xaa55
