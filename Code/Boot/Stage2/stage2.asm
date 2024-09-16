[BITS 16]
[org 0x8000]

; -------------------------
; Messages
; -------------------------
section .data
msg_a20_loading db 'A20 loading... ', 0x0D, 0x0A, 0
msg_a20_error   db 'A20 line enable -> failed ', 0x0D, 0x0A, 0
msg_a20_enable  db 'A20 line enable -> successfully ', 0x0D, 0x0A, 0

msg_gdt_loading db 'GDT loading... ', 0x0D, 0x0A, 0
msg_gdt_success db 'GDT load -> successfully ', 0x0D, 0x0A, 0

section .text

; -------------------------
; Print Message: A20 Loading
; -------------------------
mov ax, 0x0000          ; Set data segment to 0 for memory access
mov ds, ax
mov ah, 0x0E            ; BIOS teletype function (interrupt 0x10, AH=0x0E) for printing characters
mov bh, 0x00            ; Display page number

mov si, msg_a20_loading ; Load pointer to "A20 loading..." message into SI

a20_print_loading:
    lodsb               ; Load next byte from [SI] into AL
    cmp al, 0           ; Check if null terminator (end of string)
    je a20_status_check ; Jump to A20 status check if end of message
    int 0x10            ; BIOS interrupt to print character in AL
    jmp a20_print_loading ; Repeat for next character

; -------------------------
; A20 Status Check
; -------------------------
a20_status_check:
    pushf               ; Save flags
    push ds             ; Save data segment
    push es             ; Save extra segment
    push di             ; Save destination index
    push si             ; Save source index

    cli                 ; Disable interrupts during A20 check
    xor ax, ax          ; Set AX to 0
    mov es, ax          ; Set extra segment (ES) to 0
    mov di, 0x0500      ; Set DI to test memory location 0x0500 in low memory

    not ax              ; Set AX to 0xFFFF (inverted 0)
    mov ds, ax          ; Set data segment (DS) to 0xFFFF
    mov si, 0x0510      ; Set SI to memory location 0x0510 (also in low memory)

    ; Save original values from memory locations
    mov al, byte [es:di] ; Load byte at ES:DI into AL
    push ax              ; Save value from ES:DI
    mov al, byte [ds:si] ; Load byte at DS:SI into AL
    push ax              ; Save value from DS:SI

    ; Modify memory locations for the test
    mov byte [es:di], 0x00 ; Set ES:DI to 0x00
    mov byte [ds:si], 0xFF ; Set DS:SI to 0xFF

    ; Check if memory write is visible at ES:DI
    cmp byte [es:di], 0xFF ; Compare ES:DI with 0xFF

    ; Restore original memory values
    pop ax               ; Restore value from stack to AX
    mov byte [ds:si], al  ; Restore original byte at DS:SI
    pop ax               ; Restore value from stack to AX
    mov byte [es:di], al  ; Restore original byte at ES:DI

    ; Determine A20 status based on comparison result
    mov ax, 0            ; Assume A20 is off
    je status_a20_off     ; Jump if A20 is off

    mov ax, 1            ; A20 is on
    jmp status_a20_on     ; Jump to A20 enabled handling

; -------------------------
; A20 Enable via BIOS Interrupt 15h
; -------------------------
status_a20_off:
    mov ax, 0x2401       ; Request to enable A20 via BIOS (INT 15h)
    int 0x15             ; BIOS interrupt call

    jc a20_error         ; If carry flag is set (error), jump to error handler
    jmp a20_status_check ; Otherwise, recheck A20 status after enabling

; -------------------------
; A20 Enable Failure Handling
; -------------------------
a20_error:
    mov ax, 0x0000       ; Reset segment register for message output
    mov ds, ax
    mov ah, 0x0E         ; Set up for character output
    mov bh, 0x00         ; Display page number

    mov si, msg_a20_error ; Load error message into SI

print_error_loop:
    lodsb                ; Load next byte from message
    cmp al, 0            ; Check for null terminator
    je end_error_msg_a20 ; If end of string, stop printing
    int 0x10             ; Print character in AL
    jmp print_error_loop ; Loop to print the next character

end_error_msg_a20:
    cli                  ; Disable interrupts
    hlt                  ; Halt the system

; -------------------------
; A20 Enable Success Handling
; -------------------------
status_a20_on:
    mov ax, 0x0000       ; Reset data segment for message output
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

; -------------------------
; Restore Registers and Continue
; -------------------------
restore_registers_a20:
    pop si               ; Restore source index
    pop di               ; Restore destination index
    pop es               ; Restore extra segment
    pop ds               ; Restore data segment
    popf                 ; Restore flags

    sti                  ; Re-enable interrupts

; -------------------------
; GDT Loading Message
; -------------------------
mov ax, 0x0000          ; Reset data segment for message output
mov ds, ax
mov ah, 0x0E            ; Set up for character output
mov bh, 0x00            ; Display page number

mov si, msg_gdt_loading ; Load GDT loading message into SI

gdt_print_loading:
    lodsb               ; Load next byte from message
    cmp al, 0           ; Check for null terminator
    je gdt_setup        ; If end of string, jump to GDT setup
    int 0x10            ; Print character in AL
    jmp gdt_print_loading ; Loop to print the next character

; -------------------------
; GDT Setup and Loading
; -------------------------
gdt_setup:
    cli                 ; Disable interrupts during GDT setup
    lgdt [gdt_descriptor] ; Load GDT descriptor into GDTR

    ; Display success message after loading GDT
    mov ax, 0x0000       ; Reset data segment for message output
    mov ds, ax
    mov ah, 0x0E         ; Set up for character output
    mov bh, 0x00         ; Display page number

    mov si, msg_gdt_success ; Load GDT success message into SI

gdt_print_success:
    lodsb               ; Load next byte from message
    cmp al, 0           ; Check for null terminator
    je gdt_protected_mode ; If end of string, proceed to protected mode
    int 0x10            ; Print character in AL
    jmp gdt_print_success ; Loop to print the next character

; -------------------------
; Enter Protected Mode
; -------------------------
gdt_protected_mode:
    cli                 ; Disable interrupts before entering protected mode
    mov eax, cr0
    or eax, 1           ; Set protected mode bit in CR0
    mov cr0, eax

    ; Far jump to update CS with GDT code segment (0x08)
    jmp 0x08:enterProtectedMode 

[BITS 32]
enterProtectedMode:
    ; Set up segment registers for protected mode
    mov ax, 0x10        ; Load GDT data segment selector (0x10)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp kernel          ; Jump to kernel code

; -------------------------
; Kernel Code (currently halting)
; -------------------------
kernel:
    hlt                 ; Halt the CPU (placeholder for actual kernel code)

; -------------------------
; GDT Descriptor and Table
; -------------------------
section .data
gdt_start:

    ; Null descriptor (required by the processor, not used)
    dd 0x0                    ; 4 bytes: base and limit set to 0
    dd 0x0                    ; 4 more bytes: base and limit set to 0

    ; Code segment descriptor (for kernel)
    dw 0xFFFF                 ; Segment limit (16 bits) set to 0xFFFF (4GB limit with granularity)
    dw 0x0000                 ; Base address (low 16 bits) set to 0x0000
    db 0x00                   ; Base address (next 8 bits) set to 0x00
    db 10011010b              ; Access byte: 1 (present), 00 (privilege level 0), 1 (code segment), 1 (executable), 1 (readable), 0 (not accessed)
    db 11001111b              ; Flags and upper limit: 1 (granularity 4KB), 1 (32-bit op size), 0 (reserved), 0 (AVL), 1111 (upper 4 bits of limit)
    db 0x00                   ; Base address (high 8 bits) set to 0x00

    ; Data segment descriptor (for kernel)
    dw 0xFFFF                 ; Segment limit (16 bits) set to 0xFFFF (4GB limit with granularity)
    dw 0x0000                 ; Base address (low 16 bits) set to 0x0000
    db 0x00                   ; Base address (next 8 bits) set to 0x00
    db 10010010b              ; Access byte: 1 (present), 00 (privilege level 0), 1 (data segment), 0 (not executable), 1 (writable), 0 (not accessed)
    db 11001111b              ; Flags and upper limit: 1 (granularity 4KB), 1 (32-bit op size), 0 (reserved), 0 (AVL), 1111 (upper 4 bits of limit)
    db 0x00                   ; Base address (high 8 bits) set to 0x00

gdt_end:

; GDT Descriptor (contains size and location of GDT)
gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of the GDT (in bytes, minus 1)
    dd gdt_start                ; Linear address of the GDT