%ifndef KERNEL_SECTORS
%define KERNEL_SECTORS 1
%endif

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
    xor ax, ax
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
; GDT Setup and TSS Setup
; -------------------------
gdt_setup:
    cli                   ; Disable interrupts during GDT setup
    lgdt [gdt_descriptor] ; Load GDT descriptor into GDTR

    call setup_tss

; -------------------------
; GDT & TSS Success Message
; -------------------------
    xor ax, ax
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

    mov ah, 0x0E
    mov al, 'B'         ; Print 'B' for before protected mode
    int 0x10

    mov ah, 0x02        ; BIOS function to read sectors
    mov al, KERNEL_SECTORS ; Number of sectors to read
    mov ch, 0x00        ; Cylinder number (0)
    mov cl, 0x03        ; Sector number (starts at sector 3)
    mov dh, 0x00        ; Head number (0)
    mov dl, 0x80        ; Drive number (0x80 = primary hard disk)
    mov bx, load_segment      ; Destination address (0x1000 segment = 0x100000)
    int 0x13            ; Call BIOS interrupt
    jc load_error       ; Jump if error

    mov eax, cr0
    or eax, 1           ; Set protected mode bit
    mov cr0, eax

    jmp 0x08:pm_start   ; Far jump to 32-bit code
load_error:
    mov ah, 0x0E        ; BIOS function to print character
    mov al, 'E'         ; Print 'E' for error
    int 0x10            ; Call BIOS interrupt to print
    hlt

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

    mov esp, 0x9FC00    ; Set ESP to a safe location within the segment

    ; Load TSS
    mov ax, 0x28        ; TSS selector
    ltr ax              ; Load Task Register with TSS selector

    call run_offset
    jmp $
; -------------------------
; TSS Memory Allocation
; -------------------------
section .bss
    tss_start: resb 104       ; Reserve 104 bytes for the TSS
    tss_end:

section .text
global setup_tss
setup_tss:
    ; Get the base address of the TSS at runtime
    lea ax, [tss_start]          ; Load the effective address of TSS into AX

    ; Set ESP0 (kernel stack pointer) and SS0 (kernel stack selector)
    mov dword [tss_start + 4], esp0   ; Set the kernel stack pointer (ESP0)
    mov word [tss_start + 8], ss0     ; Set the kernel stack segment selector (SS0)

    ; Fill the TSS base address into GDT descriptor at runtime
    mov [gdt_tss_base], ax           ; Set the lower 16 bits (Base Low)
    shr ax, 16                       ; Get the upper 16 bits of the TSS base address
    mov [gdt_tss_base + 2], al       ; Set the middle 8 bits (Base Middle)
    mov [gdt_tss_base + 7], ah       ; Set the upper 8 bits (Base High)

    ret

section .data
esp0:    dd 0x9FC00        ; Define the kernel stack pointer (0x9FC00 is an example)
ss0:     dw 0x10           ; Define the kernel data segment selector (0x10, corresponding to your GDT)

; -------------------------
; GDT Definition
; -------------------------
gdt_start:

    ; Null Descriptor (Selector 0x00)
    dd 0x0
    dd 0x0

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
gdt_tss_base:
    dw tss_end - tss_start - 1   ; Limit (Size of TSS)
    dw 0x0000                    ; Base Low (to be filled at runtime)
    db 0x00                      ; Base Middle (to be filled at runtime)
    db 0x89                      ; Access Byte (TSS descriptor, 32-bit available)
    db 0x00                      ; Flags and Limit High
    db 0x00                      ; Base High (to be filled at runtime)

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
msg_a20_error   db 'A20 line enable                 ->   failed ', 0x0D, 0x0A, 0
msg_a20_enable  db 'A20 line enable                 ->   successfully ', 0x0D, 0x0A, 0

msg_gdtss_success db 'GDT and TSS is configured       ->   successfully ', 0x0D, 0x0A, 0
load_segment equ 0x1000
run_offset equ 0x100000