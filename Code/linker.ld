OUTPUT_FORMAT("binary")
ENTRY(_start)

SECTIONS
{
    . = 0x00100000;  /* Load the kernel at address 0x100000 */

    .text : ALIGN(0x1000) 
    {
        *(.text)
    }

    .rodata :  ALIGN(0x1000) 
    {
        *(.rodata)
    }

    .data : ALIGN(0x1000)
    {
        *(.data)
    }

    .bss : ALIGN(0x1000)
    {
        *(.bss)
        *(COMMON)
    }
}
