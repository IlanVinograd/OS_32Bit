ENTRY(_start)

KERNEL_LOW_BASE  = 0x00000000;
KERNEL_OFFSET    = 0x00100000;
KERNEL_HIGH_BASE = 0xC0000000;

SECTIONS {
    . = KERNEL_LOW_BASE + KERNEL_OFFSET;

    /* Kernel code and that will be run from low memory addresses */
    .lowentry :
    {
        *(.lowentry) /* Ensure .lowentry section is always first */
    }

    /* 0xC0000000-0xC03FFFFF now mapped to 0x00000000-0x003FFFFF.
       Note this includes the first 1MiB before the kernel. The 
       kernel itself will start at 0xC0100000
     */

    . += KERNEL_HIGH_BASE;
    .text : AT (ADDR (.text) - KERNEL_HIGH_BASE)
    {
        *(.text*)
    }
    .rodata : AT (ADDR (.rodata) - KERNEL_HIGH_BASE)
    {
        *(.rodata*)
    }
    .data : AT (ADDR (.data) - KERNEL_HIGH_BASE)
    {
        *(.data)
    }
    .bss : AT (ADDR (.bss) - KERNEL_HIGH_BASE)
    {
        *(.bss)
        *(COMMON)
    }
}
