[BITS 16]
[org 0x8000]

start:
    xor ax, ax
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

message db "|========== Hello from Stage 2! ;D ==========| ", 0

TIMES 510-($-$$) db 0
DW 0xAA55