; ------------------------------------
; Stage 2 Bootloader Code
; ------------------------------------
[BITS 16]
[ORG 0x8000]            ; Origin at physical address 0x8000 (where stage 1 loads us)

start:
    cli                 ; Disable interrupts
    cld                 ; Clear direction flag

    ; Save the boot drive number (assumed to be passed in DL by stage 1)
    mov [boot_drive], dl

    ; Print message: "Stage 2 Bootloader Started"
    mov si, msg_stage2_start
    call print_string

    ; Enable the A20 line
    call enable_a20

    ; Load the kernel from disk into memory
    call load_kernel

    ; Set up the GDT
    call setup_gdt

    ; Enter protected mode
    call enter_protected_mode

    ; Jump to the kernel entry point
    jmp KERNEL_ENTRY_POINT

; ------------------------------------
; Data and Constants
; ------------------------------------
boot_drive:      db 0                ; Storage for boot drive number (will be set by stage 1)
KERNEL_LOAD_ADDR equ 0x00100000      ; Physical address to load the kernel (1MB)
KERNEL_ENTRY_POINT equ 0x0    ; Kernel's entry point (adjust as needed)

%ifndef KERNEL_SECTORS
%define KERNEL_SECTORS 9             ; Number of sectors to read (passed from Makefile)
%endif

%ifndef KERNEL_START_LBA
%define KERNEL_START_LBA 64          ; Starting LBA of the kernel on the disk (passed from Makefile)
%endif

; ------------------------------------
; Print String Routine
; ------------------------------------
print_string:
    pusha                       ; Save all general-purpose registers
.print_char:
    lodsb                       ; Load byte at DS:SI into AL, increment SI
    or al, al                   ; Set flags based on AL
    jz .done                    ; If zero (null terminator), we're done
    mov ah, 0x0E                ; BIOS Teletype Output function
    mov bh, 0x00                ; Page number (usually 0)
    mov bl, 0x07                ; Text attribute (light grey on black)
    int 0x10                    ; BIOS video interrupt
    jmp .print_char             ; Repeat for next character
.done:
    popa                        ; Restore all general-purpose registers
    ret

; ------------------------------------
; Enable A20 Line
; ------------------------------------
enable_a20:
    ; Print message: "Enabling A20 Line..."
    mov si, msg_a20_enable
    call print_string

    ; Use BIOS function to enable A20
    mov ax, 0x2401       ; Request to enable A20 line
    int 0x15
    jc .error            ; Jump if carry flag set (error)

    ; Print message: "A20 Line Enabled Successfully"
    mov si, msg_a20_success
    call print_string

    ret

.error:
    ; Print error message
    mov si, msg_a20_error
    call print_string
    hlt

; ------------------------------------
; Load Kernel from Disk
; ------------------------------------
load_kernel:
    ; Print message: "Loading Kernel..."
    mov si, msg_load_kernel
    call print_string

    ; Set up Disk Address Packet (DAP) for int 13h extended read
    ; Since we need to load the kernel above 1MB, we must use the extended read function

    mov ah, 0x42                   ; BIOS function: Extended Read Sectors
    mov dl, [boot_drive]           ; Boot drive number
    mov si, disk_address_packet    ; Load address of DAP into SI
    int 0x13                       ; Call BIOS interrupt to read sectors
    jc .disk_error                 ; Jump if carry flag set (error)

    ; Print message: "Kernel Loaded Successfully"
    mov si, msg_kernel_loaded
    call print_string

    ret

.disk_error:
    ; Print error message
    mov si, msg_disk_error
    call print_string
    hlt

; Corrected Disk Address Packet
disk_address_packet:
    dap_size:         db 16                    ; Size of DAP (16 bytes)
    dap_reserved:     db 0                     ; Reserved, must be zero
    dap_sectors:      dw KERNEL_SECTORS        ; Number of sectors to read
    dap_buffer:       dd KERNEL_LOAD_ADDR      ; 32-bit linear address of buffer
    dap_start_lba:    dq KERNEL_START_LBA      ; Starting LBA of the kernel

; ------------------------------------
; Setup GDT for Protected Mode
; ------------------------------------
setup_gdt:
    ; Print message: "Setting Up GDT..."
    mov si, msg_setup_gdt
    call print_string

    lgdt [gdt_descriptor]  ; Load GDT descriptor into GDTR

    ; Print message: "GDT Setup Complete"
    mov si, msg_gdt_done
    call print_string

    ret

; Define the GDT
gdt_start:
    ; Null Descriptor (Selector 0x00)
    dq 0x0000000000000000

    ; Code Segment Descriptor (Selector 0x08)
    dw 0xFFFF              ; Limit Low
    dw 0x0000              ; Base Low
    db 0x00                ; Base Middle
    db 10011010b           ; Access Byte (Code Segment)
    db 11001111b           ; Flags and Limit High (4 KiB granularity, 32-bit)
    db 0x00                ; Base High

    ; Data Segment Descriptor (Selector 0x10)
    dw 0xFFFF              ; Limit Low
    dw 0x0000              ; Base Low
    db 0x00                ; Base Middle
    db 10010010b           ; Access Byte (Data Segment)
    db 11001111b           ; Flags and Limit High (4 KiB granularity, 32-bit)
    db 0x00                ; Base High
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1   ; Limit (size of GDT minus 1)
    dd gdt_start                 ; Base address of GDT

; ------------------------------------
; Enter Protected Mode
; ------------------------------------
enter_protected_mode:
    ; Print message: "Entering Protected Mode..."
    mov si, msg_enter_pm
    call print_string

    cli                           ; Disable interrupts

    ; Set PE (Protection Enable) bit in CR0 to enter protected mode
    mov eax, cr0
    or eax, 1
    mov cr0, eax

    ; Far jump to flush the prefetch queue and load new CS
    jmp 0x08:protected_mode_start

; ------------------------------------
; Protected Mode Code Segment
; ------------------------------------
[BITS 32]
protected_mode_start:
    ; Set up segment registers with data segment selector
    mov ax, 0x10                  ; Data segment selector (from GDT)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Set up stack pointer
    mov esp, 0x9FC00              ; Stack at safe memory location

    ; Since BIOS interrupts are not available in protected mode,
    ; we cannot use int 0x10 to print messages here.

    ; Jump to the kernel's entry point
    jmp KERNEL_ENTRY_POINT

; ------------------------------------
; Messages
; ------------------------------------
[BITS 16]
msg_stage2_start db "Stage 2 Bootloader Started", 0x0D, 0x0A, 0
msg_a20_enable   db "Enabling A20 Line...", 0x0D, 0x0A, 0
msg_a20_success  db "A20 Line Enabled Successfully", 0x0D, 0x0A, 0
msg_a20_error    db "A20 Line Enable Failed", 0x0D, 0x0A, 0
msg_load_kernel  db "Loading Kernel...", 0x0D, 0x0A, 0
msg_kernel_loaded db "Kernel Loaded Successfully", 0x0D, 0x0A, 0
msg_disk_error   db "Disk Read Error", 0x0D, 0x0A, 0
msg_setup_gdt    db "Setting Up GDT...", 0x0D, 0x0A, 0
msg_gdt_done     db "GDT Setup Complete", 0x0D, 0x0A, 0
msg_enter_pm     db "Entering Protected Mode...", 0x0D, 0x0A, 0

; ------------------------------------
; Fill to End of Sector (if necessary)
; ------------------------------------
times (0x7E00 - ($ - $$)) db 0   ; Pad up to 0x7E00 if needed
