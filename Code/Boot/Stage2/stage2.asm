[BITS 16]
[org 0x8000]

; -------------------------
; A20 Status Check Routine
; -------------------------
jmp a20_status_check  ; Jump to the main A20 status check routine

; -------------------------
; A20 Status Check
; -------------------------
a20_status_check:
    pushf              ; Save the flags register
    push ds            ; Save DS register
    push es            ; Save ES register
    push di            ; Save DI register
    push si            ; Save SI register

    cli                ; Disable interrupts to avoid interference during check

    xor ax, ax         ; Set AX to zero
    mov es, ax         ; ES points to segment 0x0000
    mov di, 0x0500     ; Set DI to offset 0x0500 in segment 0

    not ax             ; Set AX to 0xFFFF
    mov ds, ax         ; DS points to segment 0xFFFF
    mov si, 0x0510     ; Set SI to offset 0x0510 in segment 0xFFFF

    mov al, byte [es:di]   ; Load the byte from ES:DI (0x0000:0500) into AL
    push ax                ; Save the original byte from ES:DI

    mov al, byte [ds:si]   ; Load the byte from DS:SI (0xFFFF:0510) into AL
    push ax                ; Save the original byte from DS:SI

    mov byte [es:di], 0x00 ; Set the byte at ES:DI (0x0000:0500) to 0x00
    mov byte [ds:si], 0xFF ; Set the byte at DS:SI (0xFFFF:0510) to 0xFF

    cmp byte [es:di], 0xFF ; Compare the byte at ES:DI to 0xFF

    pop ax                 ; Restore the original byte at DS:SI
    mov byte [ds:si], al

    pop ax                 ; Restore the original byte at ES:DI
    mov byte [es:di], al

    mov ax, 0              ; Set AX to 0 (A20 disabled by default)
    je status_a20_off       ; If ES:DI is not 0xFF, A20 is disabled, jump to enable A20

    mov ax, 1              ; A20 is already enabled
    jmp status_a20_on      ; Jump to A20 enabled routine

; -------------------------
; A20 Enable via BIOS Interrupt 15h
; -------------------------
status_a20_off:
    mov ax, 0x2401         ; AX=0x2401 (Request to enable A20)
    int 0x15               ; BIOS interrupt to enable A20

    jc a20_error           ; If the carry flag is set, an error occurred

    jmp a20_status_check   ; Re-check the A20 status after enabling it

; -------------------------
; Error Handling (A20 Failed)
; -------------------------
a20_error:
    mov ax, 0x0000         ; Set DS to segment 0x0000 for printing error message
    mov ds, ax
    mov ah, 0x0E           ; BIOS Teletype service (Print character in AL)
    mov bh, 0x00           ; Set display page to 0

    mov si, msg_a20_error  ; Load the address of the error message

print_error_loop:
    lodsb                  ; Load the next byte from DS:SI into AL
    cmp al, 0              ; Check if it's the end of the message (null-terminated)
    je end_error_msg       ; If end, jump to halt the system
    int 0x10               ; Print the character in AL
    jmp print_error_loop   ; Repeat for the next character

end_error_msg:
    cli                    ; Disable interrupts
    hlt                    ; Halt the system (A20 could not be enabled)

; -------------------------
; Success Handling (A20 Enabled)
; -------------------------
status_a20_on:
    mov ax, 0x0000         ; Set DS to segment 0x0000 for printing success message
    mov ds, ax
    mov ah, 0x0E           ; BIOS Teletype service (Print character in AL)
    mov bh, 0x00           ; Set display page to 0

    mov si, msg_a20_enable ; Load the address of the success message

print_enable_loop:
    lodsb                  ; Load the next byte from DS:SI into AL
    cmp al, 0              ; Check if it's the end of the message (null-terminated)
    je restore_registers   ; If end, jump to restore registers
    int 0x10               ; Print the character in AL
    jmp print_enable_loop  ; Repeat for the next character

; -------------------------
; Restore Registers and Halt
; -------------------------
restore_registers:
    pop si                 ; Restore SI register
    pop di                 ; Restore DI register
    pop es                 ; Restore ES register
    pop ds                 ; Restore DS register
    popf                   ; Restore flags register

    cli                    ; Disable interrupts again
    hlt                    ; Halt the system

; -------------------------
; Messages
; -------------------------
msg_a20_error db 'A20 line enable -> failed ', 0
msg_a20_enable db 'A20 line enable -> successfully ', 0