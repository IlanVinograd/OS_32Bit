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
; GDT Setup
; -------------------------
gdt_setup:
    cli                   ; Disable interrupts during GDT setup
    lgdt [gdt_descriptor] ; Load GDT descriptor into GDTR

; -------------------------
; GDT Success Message
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
    mov esp, ring0_stack + 4096  ; Initialize the stack pointer

    ; Set up Ring 0 stack in TSS
    mov dword [tss32 + 4], ring0_stack + 4096  ; ESP0
    mov word [tss32 + 8], 0x10                 ; SS0

    ; Load TSS
    mov ax, 0x28          ; TSS selector
    ltr ax                ; Load Task Register with TSS selector

    jmp kernel          ; Jump to kernel code

; -------------------------
; Kernel Code
; -------------------------
kernel:
    ; Your 32-bit kernel code goes here
    sti                 ; Halt the CPU (placeholder for actual kernel code)

; -------------------------
; TSS Structure
; -------------------------
align 16
tss32:
    dw 0                ; Previous Task Link
    dw 0                ; Reserved
    dd 0                ; ESP0
    dw 0x10             ; SS0 (Ring 0 Data Segment Selector)
    dw 0                ; Reserved
    dd 0                ; ESP1
    dw 0                ; SS1
    dw 0                ; Reserved
    dd 0                ; ESP2
    dw 0                ; SS2
    dw 0                ; Reserved
    dd 0                ; CR3
    dd 0                ; EIP
    dd 0                ; EFLAGS
    dd 0                ; EAX
    dd 0                ; ECX
    dd 0                ; EDX
    dd 0                ; EBX
    dd 0                ; ESP
    dd 0                ; EBP
    dd 0                ; ESI
    dd 0                ; EDI
    dw 0                ; ES
    dw 0                ; Reserved
    dw 0                ; CS
    dw 0                ; Reserved
    dw 0                ; SS
    dw 0                ; Reserved
    dw 0                ; DS
    dw 0                ; Reserved
    dw 0                ; FS
    dw 0                ; Reserved
    dw 0                ; GS
    dw 0                ; Reserved
    dw 0                ; LDT Selector
    dw 0                ; Reserved
    dw 0                ; Trap and I/O Map Base Address
    dw 0                ; Reserved
tss32_end:

; Calculate the absolute base address of tss32
tss32_base_address equ 0x8000 + (tss32 - $$)

; Extract base address components
tss_base_low     equ tss32_base_address & 0xFFFF          ; Lower 16 bits
tss_base_middle  equ (tss32_base_address >> 16) & 0xFF    ; Next 8 bits
tss_base_high    equ (tss32_base_address >> 24) & 0xFF    ; Upper 8 bits

; -------------------------
; GDT Definition
; -------------------------
gdt_start:

    ; Null Descriptor (Selector 0x00)
    dq 0x0000000000000000

    ; Kernel Code Segment Descriptor (Selector 0x08)
    dw 0xFFFF              ; Limit Low
    dw 0x8000              ; Base Low
    db 0x00                ; Base Middle
    db 10011010b           ; Access Byte
    db 11001111b           ; Flags and Limit High
    db 0x00                ; Base High

    ; Kernel Data Segment Descriptor (Selector 0x10)
    dw 0xFFFF              ; Limit Low
    dw 0x8000              ; Base Low
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
    dw tss32_end - tss32 - 1 ; Limit
    dw tss_base_low          ; Base Low
    db tss_base_middle       ; Base Middle
    db 10001001b             ; Access Byte
    db 00000000b             ; Flags and Limit High
    db tss_base_high         ; Base High

gdt_end:

; -------------------------
; GDT Descriptor
; -------------------------
gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of the GDT (in bytes, minus 1)
    dd gdt_start                ; Linear address of the GDT

; -------------------------
; Memory Allocation
; -------------------------
section .bss
align 16
ring0_stack:
    resb 4096  ; Allocate 4KB for the Ring 0 stack

; -------------------------
; Messages
; -------------------------
msg_a20_error   db 'A20 line enable         ->   failed ', 0x0D, 0x0A, 0
msg_a20_enable  db 'A20 line enable         ->   successfully ', 0x0D, 0x0A, 0

msg_gdtss_success db 'GDT and TSS is configured       ->   successfully ', 0x0D, 0x0A, 0