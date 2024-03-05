[org 0x7c00]

mov [ BOOT_DRIVE ], dl 
KERNEL_OFFSET equ 0x1000 

; Set the stack.
mov bp , 0x9000 
mov sp , bp

mov bx , MSG_REAL_MODE
call print_string

call load_kernel

call switch_to_pm

jmp $

%include "boot/print_string.asm"
%include "boot/32_bit_print.asm"
%include "boot/print_hex.asm"
%include "boot/disk_load.asm"
%include "boot/GDT.asm"
%include "boot/switch_to_pm.asm"

[bits 16]
load_kernel:
    mov bx , KERNEL_LOAD_MSG
    call print_string

    mov bx, KERNEL_OFFSET ; Data will be stored at the offset
    mov dh, 15 
    mov dl, [BOOT_DRIVE]
    call disk_load ; loads first 15 sectors 

    ret

[bits 32]
BEGIN_PM:
    mov ebx , MSG_32_MODE
    call print_string_pm 

    call clear_screen_pm

    call KERNEL_OFFSET

    jmp $

BOOT_DRIVE: db 0
MSG_REAL_MODE: db "Starting in 16 bit real Mode ", 0
MSG_32_MODE: db "Now in 32 bit protected Mode ", 0
KERNEL_LOAD_MSG: db "Loading into the Kernel ", 0


; we are the first sector
times 510-($-$$) db 0
dw 0xaa55
