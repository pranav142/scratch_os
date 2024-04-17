[org 0x7c00]

; FAT 12 Header
; jmp over disk format information and into boot laoder
jmp short start
nop

bdb_oem:                    db 'MSWIN4.1'           ; 8 bytes
bdb_bytes_per_sector:       dw 512
bdb_sectors_per_cluster:    db 1
bdb_reserved_sectors:       dw 50
bdb_fat_count:              db 2
bdb_dir_entries_count:      dw 0E0h
bdb_total_sectors:          dw 2880                 ; 2880 * 512 = 1.44MB
bdb_media_descriptor_type:  db 0F0h                 ; F0 = 3.5" floppy disk
bdb_sectors_per_fat:        dw 9                    ; 9 sectors/fat
bdb_sectors_per_track:      dw 18
bdb_heads:                  dw 2
bdb_hidden_sectors:         dd 0
bdb_large_sector_count:     dd 0

; extended boot record
ebr_drive_number:           db 0                    ; 0x00 floppy, 0x80 hdd, useless
ebr_reserved:               db 0                    ; reserved
ebr_signature:              db 29h
ebr_volume_id:              db 12h, 34h, 56h, 78h   ; serial number, value doesn't matter
ebr_volume_label:           db 'SCRATCH OS', 0        ; 11 bytes, padded with spaces
ebr_system_id:              db 'FAT12  ', 0           ; 8 bytes

start:
    mov [ BOOT_DRIVE ], dl 
    STAGE2_OFFSET equ 0x1000

    ; Set the stack.
    mov bp , 0x9000 
    mov sp , bp

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
    
    mov bx, STAGE2_OFFSET
    mov dh, 49
    mov dl, [BOOT_DRIVE]
    call disk_load  

    ret

[bits 32]
BEGIN_PM:
    call clear_screen_pm
    
    call STAGE2_OFFSET

    jmp $

BOOT_DRIVE: db 0

; we are the first sector
times 510-($-$$) db 0
dw 0xaa55
