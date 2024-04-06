ENTRY(main)

SECTIONS
{
   . = 0x1000; 
   .text : {
        *(.text.main)
        *(.text)
    }
    .data : {
        *(.data)
    }
    .bss : {
        *(.bss)
    }
}
