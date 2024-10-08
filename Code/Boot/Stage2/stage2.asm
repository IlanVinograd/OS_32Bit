[BITS 16]
[org 0x8000]

; Kernel sectors to read
KERNEL_SECTORS     equ 48
KERNEL_LOAD_SEG    equ 0x1000
KERNEL_LOAD_OFFSET equ 0x0000
KERNEL_LOAD_ADDR   equ  ((KERNEL_LOAD_SEG<<4) + KERNEL_LOAD_OFFSET)
KERNEL_RUN_ADDR    equ 0x100000

TSS_ADDR_HI_BYTE_OFFSET equ 5
TSS_ACCESS_BYTE_OFFSET  equ 7

jmp a20_enable

; -------------------------
; BSS Section for TSS
; -------------------------
section .bss
tss resb 104    ; Reserve 104 bytes for the TSS (Task State Segment)

; -------------------------
; Initialize the TSS
; -------------------------
section .text
 
init_tss:
    ; Set ESP0 (stack pointer for privilege level 0)
    mov eax, 0x9FC00        ; Example stack pointer for ring 0
    mov [tss + 4], eax      ; Set ESP0 in the TSS
 
    ; Set SS0 (stack segment for privilege level 0)
    mov ax, 0x10            ; Kernel data segment selector (0x10)
    mov [tss + 8], ax       ; Set SS0 in the TSS
 
    ; Swap the byte with tss address bits 24:31 with the access byte
    ; In the TSS descriptor
    mov al, [gdt.tss + TSS_ADDR_HI_BYTE_OFFSET]
    xchg al, [gdt.tss + TSS_ACCESS_BYTE_OFFSET]
    mov [gdt.tss + TSS_ADDR_HI_BYTE_OFFSET], al
 
    ret                     ; Return to the main flow

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
                        ; Load 0x1000 (segment for 1MB) into AX
    mov es, ax          ; Move AX (0x1000) into ES
    mov bx, KERNEL_LOAD_OFFSET
                        ; Offset within the segment

    mov ah, 02h         ; BIOS Interrupt 13h, Function 02h: Read sectors from the disk.
    mov al, KERNEL_SECTORS
                        ; Read 1 sector from the disk (this corresponds to the size of a sector, which is 512 bytes).

    mov ch, 00h         ; Set Cylinder number to 0 (since both Stage 1 and Stage 2 are on Cylinder 0).
    mov cl, 03h         ; Set Sector number to 2 (Stage 1 is in Sector 1, so Stage 2 starts at Sector 2).
    mov dh, 00h         ; Set Head number to 0 (assuming we are using Head 0 for now).

    int 13h

    jmp gdt_setup

; -------------------------
; GDT Setup and TSS Setup
; -------------------------
gdt_setup:
    cli
    lgdt [gdt_descriptor] ; Load GDT descriptor into GDTR

    push ds
    push es
 
    call init_tss         ; Initialize the TSS
 
    pop es
    pop ds

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

    mov eax, cr0
    or eax, 1           ; Set protected mode bit in CR0
    mov cr0, eax

    jmp 0x08:update_segments
; -------------------------
; Protected Mode Code Segment
; -------------------------
[BITS 32]
update_segments:
    cli
    
    mov ax, 0x10        ; Load GDT data segment selector (0x10)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    mov ebp, 0x90000
    mov esp, ebp

    mov ax, 0x28          ; TSS selector
    ltr ax                ; Load Task Register with TSS selector

; -------------------------
; Kernel Code
; -------------------------
kernel:
    ; Copy the kernel from 0x10000 to 0x100000
    cld
    mov esi, KERNEL_LOAD_ADDR
    mov edi, KERNEL_RUN_ADDR
    mov ecx, KERNEL_SECTORS * 512
    rep movsb

    jmp 0x08:KERNEL_RUN_ADDR

; -------------------------
; GDT Descriptor and TSS
; -------------------------
section .data
gdt_start:
gdt:
     ; Null Descriptor
    dd 0x0
    dd 0x0
 
    ; Kernel Code Segment (DPL = 0)
    dw 0xFFFF              ; Limit
    dw 0x0000              ; Base (lower 16 bits)
    db 0x00                ; Base (next 8 bits)
    db 10011010b           ; Access byte
    db 11001111b           ; Flags and limit (upper 4 bits)
    db 0x00                ; Base (upper 8 bits)
 
    ; Kernel Data Segment (DPL = 0)
    dw 0xFFFF              ; Limit
    dw 0x0000              ; Base (lower 16 bits)
    db 0x00                ; Base (next 8 bits)
    db 10010010b           ; Access byte
    db 11001111b           ; Flags and limit (upper 4 bits)
    db 0x00                ; Base (upper 8 bits)
 
    ; User Code Segment (DPL = 3)
    dw 0xFFFF              ; Limit
    dw 0x0000              ; Base (lower 16 bits)
    db 0x00                ; Base (next 8 bits)
    db 11111010b           ; Access byte (DPL = 3)
    db 11001111b           ; Flags and limit (upper 4 bits)
    db 0x00                ; Base (upper 8 bits)
 
    ; User Data Segment (DPL = 3)
    dw 0xFFFF              ; Limit
    dw 0x0000              ; Base (lower 16 bits)
    db 0x00                ; Base (next 8 bits)
    db 11110010b           ; Access byte (DPL = 3)
    db 11001111b           ; Flags and limit (upper 4 bits)
    db 0x00                ; Base (upper 8 bits)
 
    ; TSS Descriptor
.tss:
    dw 0x0067                   ; Limit
    dd tss                      ; Base
    db 00000000b                ; Flags and limit (upper 4 bits)
    db 10001001b                ; Access byte
 
gdt_end:
 
; GDT Descriptor (contains size and location of GDT)
gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; Size of the GDT (in bytes, minus 1)
    dd gdt_start

; -------------------------
; Messages
; -------------------------
msg_a20_enable  db 'A20 line enable                 ->   successfully ', 0x0D, 0x0A, 0

msg_gdtss_success db 'GDT and TSS is configured       ->   successfully ', 0x0D, 0x0A, 0
