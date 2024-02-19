gdt_start:

gdt_null:
    dw 0x0000      ; Segment limit (lower 16 bits)
    dw 0x0000      ; Base address (lower 16 bits)
    db 0x00        ; Base address (middle 8 bits)
    db 0x00        ; Access byte (Type, S, DPL, P all 0)
    db 0x00        ; Granularity and limit (upper 4 bits), all 0
    db 0x00        ; Base address (upper 8 bits)

gdt_code:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0

gdt_data:
    dw 0xffff
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; size of the table
    dd gdt_start ; starting address of the table

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start