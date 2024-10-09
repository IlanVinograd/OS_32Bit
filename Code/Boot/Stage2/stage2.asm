[BITS 16]
[org 0x8000]

; Kernel sectors to read
KERNEL_SECTORS     equ 55
KERNEL_LOAD_SEG    equ 0x1000
KERNEL_LOAD_OFFSET equ 0x0000
KERNEL_LOAD_ADDR   equ  ((KERNEL_LOAD_SEG<<4) + KERNEL_LOAD_OFFSET)
KERNEL_RUN_ADDR    equ 0x100000

jmp a20_enable

; -------------------------
; A20 Enable
; -------------------------
a20_enable:
    pushf               ; Save flags
    push dx             ; Save DX register with boot drive
    push ds             ; Save data segment
    push es             ; Save extra segment
    push di             ; Save destination index
    push si             ; Save source index

    cli

    in al, 0x92
    or al, 2
    out 0x92, al

; -------------------------
; A20 Enable Msg
; -------------------------
status_a20_on:
    xor ax, ax
    mov ds, ax
    mov ah, 0x0E         ; Set up for character output
    mov bh, 0x00         ; Display page number

    mov si, msg_a20_enable ; Load success message into SI

print_enable_loop:
    lodsb                ; Load next byte from message
    cmp al, 0            ; Check for null terminator
    je restore_registers_a20 ; If end of string, restore registers
    int 0x10             ; Print character in AL
    jmp print_enable_loop ; Loop to print the next character

restore_registers_a20:
    pop si               ; Restore source index
    pop di               ; Restore destination index
    pop es               ; Restore extra segment
    pop ds               ; Restore data segment
    pop dx               ; Restore DX register
    popf                 ; Restore flags

    sti                  ; Re-enable interrupts

; -------------------------
; Load kernel from disk
; -------------------------
    mov ax, KERNEL_LOAD_SEG
    mov es, ax          ; ES = KERNEL_LOAD_SEG
    mov bx, KERNEL_LOAD_OFFSET

    mov ah, 02h         ; BIOS Interrupt 13h, Function 02h: Read sectors from the disk.
    mov al, KERNEL_SECTORS      ; Number of sectors to read

    mov ch, 00h         ; Cylinder number
    mov cl, 03h         ; Sector number (starts from 1)
    mov dh, 00h         ; Head number
    mov dl, 0x80        ; Boot drive (assuming first hard disk)

    int 13h             ; Call BIOS to read sectors

    jc disk_read_error  ; If carry flag is set, jump to error handler

    jmp enter_protected_mode  ; Proceed to enter protected mode

disk_read_error:
    ; Handle disk read error
    mov si, msg_disk_read_error
    call print_string
    hlt

; -------------------------
; Enter Protected Mode
; -------------------------
enter_protected_mode:
    cli                 ; Disable interrupts

    lgdt [gdt_descriptor] ; Load GDT descriptor into GDTR

    mov eax, cr0
    or eax, 1           ; Set PE bit (Protection Enable)
    mov cr0, eax

    ; Far jump to flush prefetch queue and enter protected mode
    jmp 0x08:protected_mode_entry

; -------------------------
; Protected Mode Entry Point
; -------------------------
[BITS 32]
protected_mode_entry:
    cli
    ; Set up segment registers
    mov ax, 0x10        ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up stack
    mov ebp, 0x90000
    mov esp, ebp

    ; Copy kernel from KERNEL_LOAD_ADDR to KERNEL_RUN_ADDR
    cld
    mov esi, KERNEL_LOAD_ADDR
    mov edi, KERNEL_RUN_ADDR
    mov ecx, KERNEL_SECTORS * 512   ; Number of dwords
    rep movsb

    ; Jump to kernel entry point
    jmp 0x08:KERNEL_RUN_ADDR

; -------------------------
; GDT Setup
; -------------------------
section .data
gdt_start:
gdt:
    ; Null Descriptor
    dd 0x0
    dd 0x0

    ; Code Segment Descriptor (selector 0x08)
    dw 0xFFFF              ; Limit Low
    dw 0x0000              ; Base Low
    db 0x00                ; Base Middle
    db 10011010b           ; Access byte: Present, Ring 0, Code segment, Executable, Readable
    db 11001111b           ; Flags and Limit High: Granularity, 32-bit
    db 0x00                ; Base High

    ; Data Segment Descriptor (selector 0x10)
    dw 0xFFFF              ; Limit Low
    dw 0x0000              ; Base Low
    db 0x00                ; Base Middle
    db 10010010b           ; Access byte: Present, Ring 0, Data segment, Writable
    db 11001111b           ; Flags and Limit High: Granularity, 32-bit
    db 0x00                ; Base High

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start -1    ; Limit (size of GDT -1)
    dd gdt_start                 ; Base address of GDT

; -------------------------
; Print String Function
; -------------------------
; Assumes DS is set properly
print_string:
    mov ah, 0x0E         ; BIOS teletype function
.print_loop:
    lodsb
    cmp al, 0
    je .print_done
    int 0x10
    jmp .print_loop
.print_done:
    ret

; -------------------------
; Messages
; -------------------------
msg_a20_enable db 'A20 line enabled successfully', 0x0D, 0x0A, 0
msg_disk_read_error db 'Disk read error!', 0x0D, 0x0A, 0