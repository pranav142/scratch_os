org 0x7c00

; this program is loaded on a non volatile bootable device, the drive that it is loaded from is stored in dl
mov [BOOT_DRIVE], dl ; dl contains the drive number that this program is being booted in
; we want to store this value in Boot drive to keep track of the drive we booted from

mov bp, 0x8000 ; set our base pointer far away
mov sp, bp

mov bx, 0x9000 ; where we want to store our data
mov dh, 5 ; read five sectors
mov dl, [BOOT_DRIVE]
call disk_load

mov dx, [0x9000] ; reads the first two bytes (16 bits) from the second sector
call print_hex

mov dx, [0x9000 + 512] ; reads the value from the second sector
call print_hex

jmp $ ; loops forever program jumps to current instruction

; make sure to put this below the jmp $ otherwise at the end of the program this will prematurely 
; be run despite never being called
%include "print_string.asm"
%include "print_hex.asm"
%include "disk_load.asm"

BOOT_DRIVE: db 0 ; the device we are booting from

; we are the first sector
times 510-($-$$) db 0
dw 0xaa55

; sector 2
dw 0xdada
times 510 db 0
; sector 3
dw 0xface
times 510 db 0