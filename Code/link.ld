OUTPUT_FORMAT(binary)

ENTRY(_start)

SECTIONS
{
    . = 0x100000;

    .text32 : {
        *(.text)
    }
    .data32 : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}