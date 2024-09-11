[BITS 16]
[ORG 0x8000]

start:
    mov si, message
    call print_string

    cli
    hlt

print_string:
    mov ah, 0x0E
.next_char:
    lodsb
    cmp al, 0
    je done
    int 0x10
    jmp .next_char

done:
    ret

message db "Hello from Stage 2! :)", 0

TIMES 510-($-$$) db 0
DW 0xAA55
