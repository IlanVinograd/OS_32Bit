ENTRY(_start)

LOW_BASE = 0x100000;
HIGH_BASE = 0xC0000000;

SECTIONS {
    . = LOW_BASE;

    .lowtext :
    {
        *(.lowentry) /* Ensure .lowentry section is always first */
        *(.lowtext)
    }
    .lowdata :
    {
        *(.lowdata)
    }
    lower_size = . - LOW_BASE;

    lower_higher_diff = HIGH_BASE - . + lower_size;
    . = HIGH_BASE + lower_size;

    /* Add a symbol that indicates the start address of the kernel. */
    .text : AT (ADDR (.text) - lower_higher_diff)
    {
        *(.text*)
    }
    .rodata : AT (ADDR (.rodata) - lower_higher_diff)
    {
        *(.rodata*)
    }
    .data : AT (ADDR (.data) - lower_higher_diff)
    {
        *(.data)
    }
    .bss : AT (ADDR (.bss) - lower_higher_diff)
    {
        *(.bss)
        *(COMMON)
    }
}
