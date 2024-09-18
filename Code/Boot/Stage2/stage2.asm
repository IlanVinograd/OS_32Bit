[BITS 16]
[org 0x8000]

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
    xor ax, ax           ; Reset segment register for message output
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
    hlt                  ; Halt the system

; -------------------------
; A20 Enable Success Handling
; -------------------------
status_a20_on:
    xor ax, ax           ; Reset data segment for message output
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

    ; Now proceed to set up GDT and TSS
    jmp gdt_setup

; -------------------------
; GDT And TSS Setup
; -------------------------
gdt_setup:
    cli                   ; Disable interrupts during GDT setup
    lgdt [gdt_descriptor] ; Load GDT descriptor into GDTR

    ; Now load the TSS
    mov ax, 0x28          ; TSS selector (assuming it's at index 5)
    ltr ax                ; Load Task Register with TSS selector

; -------------------------
; GDT & TSS Success Message
; -------------------------
    xor ax, ax       ; Reset data segment for message output
    mov ds, ax
    mov ah, 0x0E         ; Set up for character output
    mov bh, 0x00         ; Display page number

    mov si, msg_gdtss_success ; Load GDT success message into SI

gdtss_print_success:
    lodsb                   ; Load next byte from message
    cmp al, 0               ; Check for null terminator
    je enter_protected_mode     ; If end of string, proceed to protected mode
    int 0x10                ; Print character in AL
    jmp gdtss_print_success   ; Loop to print the next character

; -------------------------
; Enter Protected Mode
; -------------------------
enter_protected_mode:
    cli                 ; Disable interrupts before entering protected mode
    mov eax, cr0
    or eax, 1           ; Set protected mode bit in CR0
    mov cr0, eax

    ; Far jump to update CS with GDT code segment (0x08)
    jmp 0x08:pm_start

; -------------------------
; Protected Mode Code Segment
; -------------------------
[BITS 32]
pm_start:
    ; Set up segment registers for protected mode
    mov ax, 0x10        ; Load GDT data segment selector (0x10)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Continue with your kernel code
    jmp kernel          ; Jump to kernel code

; -------------------------
; Kernel Code
; -------------------------
kernel:
    ; Your 32-bit kernel code goes here
    hlt                 ; Halt the CPU (placeholder for actual kernel code)

; -------------------------
; TSS Structure
; -------------------------
tss32:
    ; The TSS structure begins here
    dw 0                ; Previous Task Link
    dw 0
    dd 0                ; esp0 (Ring 0 Stack Pointer)
    dd 0                ; ss0 (Ring 0 Stack Segment)
    dd 0                ; esp1
    dd 0                ; ss1
    dd 0                ; esp2
    dd 0                ; ss2
    dd 0                ; cr3
    dd 0                ; eip
    dd 0                ; eflags
    dd 0                ; eax
    dd 0                ; ecx
    dd 0                ; edx
    dd 0                ; ebx
    dd 0                ; esp
    dd 0                ; ebp
    dd 0                ; esi
    dd 0                ; edi
    dd 0                ; es
    dd 0                ; cs
    dd 0                ; ss
    dd 0                ; ds
    dd 0                ; fs
    dd 0                ; gs
    dd 0                ; ldtr
    dw 0                ; Trap
    dw 0x0000           ; I/O Map Base Address
tss32_end:

[BITS 16]   ; Switch back to 16 bits for the rest

; -------------------------
; GDT Definition
; -------------------------
gdt_start:

    ; Null Descriptor (Selector 0x00)
    dq 0x0000000000000000

    ; Kernel Code Segment Descriptor (Selector 0x08)
    dw 0xFFFF              ; Limit Low
    dw 0x0000              ; Base Low
    db 0x00                ; Base Middle
    db 10011010b           ; Access Byte
    db 11001111b           ; Flags and Limit High
    db 0x00                ; Base High

    ; Kernel Data Segment Descriptor (Selector 0x10)
    dw 0xFFFF              ; Limit Low
    dw 0x0000              ; Base Low
    db 0x00                ; Base Middle
    db 10010010b           ; Access Byte
    db 11001111b           ; Flags and Limit High
    db 0x00                ; Base High

    ; User Code Segment Descriptor (Selector 0x18)
    dw 0xFFFF              ; Limit Low
    dw 0x0000              ; Base Low
    db 0x00                ; Base Middle
    db 11111010b           ; Access Byte
    db 11001111b           ; Flags and Limit High
    db 0x00                ; Base High

    ; User Data Segment Descriptor (Selector 0x20)
    dw 0xFFFF              ; Limit Low
    dw 0x0000              ; Base Low
    db 0x00                ; Base Middle
    db 11110010b           ; Access Byte
    db 11001111b           ; Flags and Limit High
    db 0x00                ; Base High

    ; TSS Descriptor (Selector 0x28)
    ; Calculate TSS Base and Limit
tss_base   equ tss32
tss_limit  equ tss32_end - tss32 - 1

    dw tss_limit & 0xFFFF          ; Limit Low
    dw tss_base & 0xFFFF           ; Base Low
    db (tss_base >> 16) & 0xFF     ; Base Middle
    db 10001001b                   ; Access Byte (Present, Ring 0, Type 1001b = 32-bit Available TSS)
    db ((tss_limit >> 16) & 0x0F) | (0 << 4) ; Flags (Granularity = 0) and Limit High
    db (tss_base >> 24) & 0xFF     ; Base High

gdt_end:

; -------------------------
; GDT Descriptor
; -------------------------
gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of the GDT (in bytes, minus 1)
    dd gdt_start                ; Linear address of the GDT

; -------------------------
; Messages
; -------------------------
msg_a20_error   db 'A20 line enable         ->   failed ', 0x0D, 0x0A, 0
msg_a20_enable  db 'A20 line enable         ->   successfully ', 0x0D, 0x0A, 0

msg_gdtss_success db 'GDT and TSS configured  ->   successfully ', 0x0D, 0x0A, 0