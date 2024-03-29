disk_load:
    ; dh specifies how many sectors to read ; dl specifies the device to read from 0x00 is floppy
    push dx
    
    mov ah, 0x02 ; Tells BIOS we want to Read from a Device
    ; dl would specify which device to read from
    mov al, dh   ; number of sectors to read
    mov ch, 0x00 ; Choose cylinder 0
    mov dh, 0x00 ; read from the first head
    mov cl, 0x02 ; read from the second sector

    int 0x13

    jc disk_error ; if a carry flag is raised by the bios during this operation print error

    pop dx
    ret

disk_error: 
    mov bx, DISK_ERROR_MSG
    call print_string
    jmp $

DISK_ERROR_MSG db "Disk read error!", 0
