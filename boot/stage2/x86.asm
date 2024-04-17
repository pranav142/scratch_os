%macro EnterRealMode 0
    [bits 32]
    cli
    push eax
    jmp word 18h:.pmode16

.pmode16:
    [bits 16]

    mov eax, cr0
    and al, ~1 
    mov cr0, eax
    
    jmp word 00h:.rmode

.rmode:
    mov ax, 0
    mov ds, ax
    mov ss, ax
    
    pop eax
    sti

%endmacro


%macro EnterProtectedMode 0
    cli
    push eax

    mov eax, cr0
    or al, 1
    mov cr0, eax

    jmp dword 08h:.pmode

.pmode:
    [bits 32]
    
    mov ax, 0x10
    mov ds, ax
    mov ss, ax

    pop eax

%endmacro

; bool __attribute__((cdecl)) disk_reset(uint8_t drive);
global x86_disk_reset
x86_disk_reset:
    push ebp
    mov ebp, esp
    push edx

    EnterRealMode

    mov ah, 0
    mov dl, [ebp + 8]

    int 0x13
    
    jc .error
    mov eax, 0
    jmp .done

.error:
    mov eax, 1
    
.done:
    EnterProtectedMode
    
    pop edx
    mov esp, ebp
    pop ebp
    ret

;bool __attribute__((cdecl)) disk_read(uint8_t drive, uint8_t sector,
;                                     uint8_t sector_count, uint8_t cylinder,
;                                     uint8_t head, uint16_t linear_segment,
;                                     uint16_t linear_offset);

global x86_disk_read
x86_disk_read:
    push ebp
    mov ebp, esp
    push edx
    push ecx
    push ebx
    push es

    EnterRealMode

    ; AH = 0x02 for read, AL = sector count
    mov ah, 0x02
    mov al, [ebp + 16] ; sector_count
    mov dl, [ebp + 8]  ; drive
    mov cl, [ebp + 12] ; sector
    mov dh, [ebp + 24] ; head
    mov ch, [ebp + 20] ; cylinder
    
    mov bx, [ebp + 28] ; linear_segment
    mov es, bx
    mov bx, [ebp + 32] ; linear_offset
    
    int 0x13
    
    jc .error
    mov eax, 0
    jmp .done

.error:
    mov eax, 1

.done:
    EnterProtectedMode
    
    pop es
    pop ebx
    pop ecx
    pop edx
    mov esp, ebp
    pop ebp
    ret


; bool __attribute__((cdecl)) get_disk_params(uint8_t drive, uint8_t &drive_type_out, uint16_t &cylinders_out, uint16_t &sectors_out, uint16_t &heads_out);
global x86_get_disk_params
x86_get_disk_params:
    push ebp
    mov ebp, esp

    push es
    push edx
    push ecx
    push ebx
    push esi
    push di

    EnterRealMode

    mov ah, 0x08
    mov dl, [ebp + 8]  ; drive
        
    mov di, 0
    mov es, di

    int 0x13
    
    mov eax, 0
    jc .error

    jmp .done

.error:
    mov eax, 1

.done:
    EnterProtectedMode
    
    mov esi, [ebp + 12]
    mov [esi], bl

    mov bl, ch          ; cylinders - lower bits in ch
    mov bh, cl          ; cylinders - upper bits in cl (6-7)
    shr bh, 6
    inc bx
    mov esi, [ebp + 16]
    mov [esi], bx

    xor bx, bx
    mov bl, cl
    and bl, 0x3f        ; bits 0-5 are max sectors
    mov esi, [ebp + 20]
    mov [esi], bx

    xor cx, cx
    mov cl, dh          ; heads - dh
    inc cx
    mov esi, [ebp + 24]
    mov [esi], cx

    pop di
    pop esi
    pop ebx
    pop ecx
    pop edx
    pop es

    mov esp, ebp
    pop ebp
    ret

;void __attribute__((cdecl)) x86_call_kernel_start(uint32_t kernel_addr);
global x86_call_kernel_start
x86_call_kernel_start:  
    cli
    jmp 0x10000


