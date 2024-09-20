ENTRY(start)

SECTIONS
{
    /* Place the kernel at the 0x10000 address */
    . = 0x10000;

    /* The text section for the code */
    .text : {
        *(.text)
    }

    /* The data section for initialized data */
    .data : {
        *(.data)
    }

    /* The BSS section for uninitialized data */
    .bss : {
        *(.bss)
    }
}
