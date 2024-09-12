[BITS 16]
[org 0x0000]

start:
    mov ax, 0x0800
    mov ds, ax

    mov si, message
    call print_string

    cli
    hlt

print_string:
    mov ah, 0x0E
    mov bl, 0x02
.next_char:
    lodsb
    cmp al, 0
    je done
    mov bh, 0x00
    int 0x10
    jmp .next_char

done:
    ret

message db "Hello from Stage 2! :)", 0

TIMES 510-($-$$) db 0
DW 0xAA55