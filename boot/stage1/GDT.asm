gdt_start:

gdt_null:
    dw 0x0000      ; Segment limit (lower 16 bits)
    dw 0x0000      ; Base address (lower 16 bits)
    db 0x00        ; Base address (middle 8 bits)
    db 0x00        ; Access byte (Type, S, DPL, P all 0)
    db 0x00        ; Granularity and limit (upper 4 bits), all 0
    db 0x00        ; Base address (upper 8 bits)

; these two segments can access the same physical but with different purposes
; read each byte individually from left to right
gdt_code:
    ; Base Address = 0x0, Segment Limit = 0xFFFFF, Segment Size ~4GiB
    ; Type:  E=1 Code segment, A=0 segment is not being accessed, RW=1 writable, DC=0 Segment Can Only Be Run by Process of Same Privelege level
    ; Flags: S=1 Code or Data Segment not system segment, DPL=00 Highest Privelege, P=1 Present in memory
    ; Flags: G=1 Limit is Scaled by 4kB, DB=1 32 Bit Protected Segment, L=0 Not 64 bit
    ; AVL is a reserved Flag that can be adjusted by the system 
    dw 0xffff      ; Lower 16 bits of Segment Limit 
    dw 0x0         ; Lower 16 Bits of Base Address
    db 0x0         ; middle 8 Bits of Base Address
    db 10011010b   ; Type (A=0, RW=1, DC=0, E=1) S: 1, DPL: 00, P: 1
    db 11001111b   ; Upper 4 bits Segment Limit = 1111, AVL: 0, L: 0, DB: 1, G: 1
    db 0x0         ; upper 8 Bits of Base Address

gdt_data:
    ; Base Address = 0x0, Segment Limit = 0xFFFFF, Segment Size ~4GiB
    ; Type:  E=0 Data segment, A=0 segment is not being accessed, RW=1 readable, DC=0 Segment Grows Up
    ; Flags: S=1 Code or Data Segment not system segment, DPL=00 Highest Privelege, P=1 Present in memory
    ; Flags: G=1 Limit is Scaled by 4kB, DB=1 32 Bit Protected Segment, L=0 Not 64 bit
    ; AVL is a reserved Flag that can be adjusted by the system 
    dw 0xffff      ; lower 16 bits of Segment Limit
    dw 0x0         ; Lower 16 bits of Base Address
    db 0x0         ; middle 8 bits of Base Address
    db 10010010b   ; Type (A=0, RW=1, DC=0, E=0) S: 1, DPL: 00, P: 1
    db 11001111b   ; Upper 4 bits Segment Limit = 1111, AVL: 0, L: 0, DB: 1, G: 1
    db 0x0         ; Upper 8 Bits of Base Address

gdt_code_real:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 00001111b
    db 0x0

gdt_data_real:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 00001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size of the table
    dd gdt_start ; starting address of the table

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start
