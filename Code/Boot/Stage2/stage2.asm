%ifndef KERNEL_SECTORS
%define KERNEL_SECTORS 1 ; default value if not defined
%endif

[BITS 16]                ; Real mode, 16-bit code
[ORG 0x0000]
global a20_status_check
; -------------------------
; A20 Status Check (16-bit code)
; -------------------------
section .text16
a20_status_check:
    ; Save registers
    pushf
    push ds
    push es
    push di
    push si

    ; Disable interrupts
    cli
    xor ax, ax          ; Set AX to 0
    mov es, ax          ; Set extra segment (ES) to 0
    mov di, 0x0500      ; Set DI to memory location 0x0500 (low memory)

    not ax              ; Set AX to 0xFFFF
    mov ds, ax          ; Set data segment (DS) to 0xFFFF
    mov si, 0x0510      ; Set SI to memory location 0x0510 (low memory)

    ; Save original values from memory locations
    mov al, byte [es:di]
    push ax
    mov al, byte [ds:si]
    push ax

    ; Modify memory locations for the test
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xFF

    ; Check if memory write is visible at ES:DI
    cmp byte [es:di], 0xFF

    ; Restore original memory values
    pop ax
    mov byte [ds:si], al
    pop ax
    mov byte [es:di], al

    ; Determine A20 status based on comparison result
    mov ax, 0
    je status_a20_off     ; If memory mismatch, A20 is off
    mov ax, 1
    jmp status_a20_on     ; If match, A20 is on

; -------------------------
; A20 Enable Handling (16-bit code)
; -------------------------
status_a20_off:
    mov ax, 0x2401        ; Request to enable A20 via BIOS (INT 15h)
    int 0x15
    jc a20_error          ; Jump if BIOS failed to enable A20
    jmp a20_status_check  ; Retry checking A20 status

a20_error:
    ; Print A20 error message and halt
    call print_error_msg
    hlt

status_a20_on:
    ; Print A20 enable success message
    call print_success_msg
    jmp gdt_setup         ; Proceed to GDT setup

; -------------------------
; Printing Functions (16-bit code)
; -------------------------
print_error_msg:
    mov si, msg_a20_error
    call print_string
    ret

print_success_msg:
    mov si, msg_a20_enable
    call print_string
    ret

print_string:
    xor ax, ax
    mov ds, ax
    mov ah, 0x0E           ; BIOS teletype function for printing
    mov bh, 0x00
print_loop:
    lodsb                  ; Load byte from string into AL
    cmp al, 0              ; Check for null terminator
    je print_done
    int 0x10               ; Print character
    jmp print_loop
print_done:
    ret

; -------------------------
; GDT Setup (16-bit code)
; -------------------------
gdt_setup:
    cli                    ; Disable interrupts during setup
    lgdt [gdt_descriptor]   ; Load GDT descriptor into GDTR
    call setup_tss          ; Set up the TSS
    jmp enter_protected_mode ; Jump to protected mode

; -------------------------
; Enter Protected Mode (16-bit code)
; -------------------------
enter_protected_mode:
    cli
    mov eax, cr0
    or eax, 1              ; Set protected mode bit (PE bit) in CR0
    mov cr0, eax

    ; Far jump to reload CS with GDT code segment selector (0x08)
    jmp 0x08:pm_start

; -------------------------
; GDT Descriptor and TSS Setup (16-bit code)
; -------------------------
global setup_tss
setup_tss:
    lea ax, [tss_start]         ; Load effective address of TSS into AX
    mov [gdt_tss_base], ax      ; Set lower 16 bits of TSS base in GDT entry
    shr ax, 16                  ; Get the upper 16 bits
    mov [gdt_tss_base+2], al
    mov [gdt_tss_base+7], ah
    ret

; -------------------------
; Transition to 32-bit Code
; -------------------------
[BITS 32]                ; Protected mode, 32-bit code
section .text32
%include "ATA_PIO.asm"
; -------------------------
; Protected Mode Start (32-bit code)
; -------------------------
pm_start:
    ; Set up data segments for 32-bit mode
    mov ax, 0x10            ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9FC00  
    ; Set up stack pointer (optional, based on your setup)
    ; mov esp, 0x9FC00      ; Set ESP to a safe location

    ; Load Task State Segment (TSS)
    mov ax, 0x28            ; TSS selector
    ltr ax                  ; Load Task Register

    jmp loadKernel         ; Proceed to kernel load

; -------------------------
; Kernel Code (32-bit code)
; -------------------------
loadKernel: 
	mov ebp, 3               ; Starting sector (matching seek=14 in dd)
    mov ebx, KERNEL_SECTORS   ; Number of sectors to read
    mov edi, 0x100000         ; Address to load the kernel in memory
    call read_ata_st
    jmp 0x100000     
; -------------------------
; GDT Definitions (Data Section)
; -------------------------
section .data
gdt_start:

    ; Null Descriptor (0x00)
    dd 0x0
    dd 0x0

    ; Kernel Code Segment (0x08)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10011010b
    db 11001111b
    db 0x00

    ; Kernel Data Segment (0x10)
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 10010010b
    db 11001111b
    db 0x00

    ; TSS Descriptor (0x28)
gdt_tss_base:
    dw tss_end - tss_start - 1
    dw 0x0000
    db 0x00
    db 0x89
    db 0x00
    db 0x00

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of GDT
    dd gdt_start                ; Address of GDT

; -------------------------
; TSS Memory Allocation (BSS Section)
; -------------------------
section .bss
tss_start: resb 104  ; Reserve 104 bytes for TSS
tss_end:

; -------------------------
; Messages (Data Section)
; -------------------------
msg_a20_error   db 'A20 line enable failed', 0x0D, 0x0A, 0
msg_a20_enable  db 'A20 line enabled successfully', 0x0D, 0x0A, 0
