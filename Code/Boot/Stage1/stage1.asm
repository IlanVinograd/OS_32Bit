[BITS 16]               ; We are working in 16-bit Real Mode
[org 0x7c00]            ; The origin (starting address) of the bootloader in memory, which is 0x7C00 as loaded by the BIOS.
                        ; This is the physical address where the bootloader is loaded into memory.

start:                  ; Start of execution, this label marks the entry point of the code.
    jmp main            ; Jump to the 'main' label to start execution.

main:                   ; Main routine of the bootloader begins here.

; -------------------------
; Setup segment registers
; -------------------------
    cli                 ; Clear interrupts to ensure no interrupts occur while setting up segments.
    xor ax, ax          ; Set AX to 0x0 (which is 0x0 >> 4).
                        ; Explanation: We are using segment:offset addressing in real mode.
                        ; Physical address = Segment * 16 + Offset
                        ; So, the segment 0x0 * 16 = 0x0 (physical address).
    mov ds, ax          ; Set Data Segment (DS) to 0x0. DS points to the bootloader code/data in memory.
    mov es, ax          ; Set Extra Segment (ES) to 0x0. ES is also set to point to our code/data.

; -------------------------
; Setup stack
; -------------------------
    mov ss, ax          ; Set Stack Segment (SS) to 0 (base of memory).
    mov sp, 0xFFFE      ; Set the Stack Pointer (SP) to the highest address within the current 64KB segment (0x0000:0xFFFE).
                        ; In real mode, the stack grows downward from 0xFFFE and 0xFFFE should be set to an even offset like 0xFFFE, not an odd one.

    sti                 ; Re-enable interrupts after segment and stack setup is complete.

; -------------------------
; Load Stage 2 bootloader from disk
; -------------------------
    mov ah, 02h         ; BIOS Interrupt 13h, Function 02h: Read sectors from the disk.
    mov al, 63h         ; Read 1 sector from the disk (this corresponds to the size of a sector, which is 512 bytes).

    mov ch, 00h         ; Set Cylinder number to 0 (since both Stage 1 and Stage 2 are on Cylinder 0).
    mov cl, 02h         ; Set Sector number to 2 (Stage 1 is in Sector 1, so Stage 2 starts at Sector 2).
    mov dh, 00h         ; Set Head number to 0 (assuming we are using Head 0 for now).
    mov dl, 80h         ; Use the first hard drive (usually 0x80 for the primary hard disk).

    mov bx, 0x8000      ; Set BX to 0x8000, the offset address where Stage 2 will be loaded.
                        ; Stage 2 will be loaded into memory using segment:offset addressing.
                        ; ES = 0x0, BX = 0x8000, so the physical address = ES * 16 + BX.
                        ; Formula: 0x0 * 16 + 0x8000 = 0x0 + 0x8000 = 0x8000 (the physical address where Stage 2 is loaded).

    int 13h             ; Call BIOS interrupt 13h to read the specified sectors into memory.

    jc disk_read_error  ; If carry flag is set (indicating an error), jump to the error handler.

pass:                   ; If the disk read was successful (carry flag is cleared), continue from here.
    jmp 0x0000:0x8000   ; Jump to the loaded Stage 2 at address 0x0000:0x8000 (this is where Stage 2 resides).
                        ; Here, 0x0000 is the segment, and 0x8000 is the offset.
                        ; Physical address = 0x0000 * 16 + 0x8000 = 0x8000, where Stage 2 is loaded.

disk_read_error:
    int 18h             ; If the disk read fails, call INT 18h to attempt a boot from a different device (like network boot).
                        ; This error message will occur --> IO write(0x01f0): current command is 20h displayed on bochs emulator.

TIMES 510-($-$$) DB 0   ; Pad the bootloader to ensure it is exactly 512 bytes, with zeros filling the remaining space.
DW 0xAA55               ; The boot signature (magic number) required for the BIOS to recognize this as a bootable sector.