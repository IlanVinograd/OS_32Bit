.section .text
.global lgdt
.type lgdt, @function

lgdt:
    movl 4(%esp), %eax    # Load pointer to gdt_ptr
    lgdt (%eax)
    ret

