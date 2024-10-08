.section .lowentry, "ax"
.extern _start
.extern init_paging

.set KERNEL_HIGH_BASE, 0xc0000000
.set KERNEL_LOW_BASE, 0x00100000
.set KERNEL_OFFSET_DIFF, (KERNEL_HIGH_BASE-KERNEL_LOW_BASE)

_entry:
    # Init paging first while running in lower half addreses
    call init_paging

    # Init stack to higher half addresses
    mov $stack_top, %esp

    # _start label is a higher half address. After this point
    # we are running in the higher half of memory.
    call _start

hltloop:
    hlt
    jmp hltloop

# 8k stack aligned to 16 bytes in .bss section
.local stack_top, stack_bottom
.comm stack_bottom, 8192, 16
.comm stack_top, 0

