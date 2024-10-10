.section .lowentry, "ax"
.extern _start
.extern init_paging

.set KERNEL_HIGH_BASE, 0xC0000000
.set KERNEL_LOW_BASE, 0x00000000
.set KERNEL_OFFSET_DIFF, (KERNEL_HIGH_BASE - KERNEL_LOW_BASE)

.global _entry

_entry:
    # Stage2.asm sets the initial stack at 0x90000. Use that
    # stack until we have mapped the higher half addresses

    # The only function that needs to be called before _start
    # is init_paging. Everything else can be done from within
    # _start.

    # Initialize paging. init_paging is given a higher half address
    # so we adjust it to its lower half address
    call init_paging - KERNEL_HIGH_BASE

    # Now that the higher half is mapped we can use our higher
    # half stack
    mov $kernel_stack_top, %esp

    # Now call the kernel's main function (_start) at higher-half address
    call _start

hltloop:
    hlt
    jmp hltloop

# linker.ld places .bss section in higher half.

# Kernel stack in higher half address space
# Aligning stacks on 4k page boundaries can be useful in the future
.section .bss
.global kernel_stack_bottom
.global kernel_stack_top
.align 4096
kernel_stack_bottom:
    .skip 8192          # 8 KB stack
kernel_stack_top:

# Separate TSS stack for transitions to ring 0
.global esp0_stack_bottom
.global esp0_stack_top
.align 4096
esp0_stack_bottom:
    .skip 8192          # 8 KB stack
esp0_stack_top:

.global page_directory_low
.set page_directory_low, page_directory - KERNEL_HIGH_BASE
.align 4096
page_directory:
    .skip 4096

.global first_page_table_low
.set first_page_table_low, first_page_table - KERNEL_HIGH_BASE
.align 4096
first_page_table:
    .skip 4096

.global new_page_table_low
.set new_page_table_low, new_page_table - KERNEL_HIGH_BASE
.align 4096
new_page_table:
    .skip 4096
