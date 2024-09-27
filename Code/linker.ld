ENTRY(_start)
SECTIONS
{
    . = 0x100000;
    
    .text : {
        *(.entry) /* Ensure .entry section is always first */
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}