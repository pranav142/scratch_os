OUTPUT_FORMAT(elf32-i386)
ENTRY(kernel_start)
SECTIONS
 {
    . = 0x100000;
    .text : {
        *(.text.main)
        *(.text)
    }
    .data : {*(.data)}
    .bss  : {*(.bss)}
 }
