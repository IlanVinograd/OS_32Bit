ENTRY(_start)

SECTIONS
{
    . = 0x8000;

    .text16 : {
        *(.text16)
    }

    . = ALIGN(16);

    .text32 : {
        *(.text32)
    }
    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}