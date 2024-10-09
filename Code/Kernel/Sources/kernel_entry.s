.section .lowentry, "ax"
.extern _start
.extern init_gdt
.extern init_paging

.set KERNEL_HIGH_BASE, 0xC0000000
.set KERNEL_LOW_BASE, 0x00100000
.set KERNEL_OFFSET_DIFF, (KERNEL_HIGH_BASE - KERNEL_LOW_BASE)

.global _entry

_entry:
    # Set up the stack using physical addresses
    mov $phys_stack_top, %esp

    # Initialize the GDT
    call init_gdt

    # Initialize paging
    call init_paging

    # Adjust stack pointer for higher-half addresses
    add $(KERNEL_OFFSET_DIFF), %esp

    # Now call the kernel's main function (_start) at higher-half address
    call _start

hltloop:
    hlt
    jmp hltloop

# Stack definition in physical memory
.section .bss
.align 16
.global phys_stack_bottom
.global phys_stack_top
phys_stack_bottom:
    .skip 8192          # 8 KB stack
phys_stack_top:

# Define stack_top as a label at the higher-half address
.section .text
.global stack_top
stack_top:

# Adjust the location of stack_top to be at the higher-half address
# The linker will handle the relocation