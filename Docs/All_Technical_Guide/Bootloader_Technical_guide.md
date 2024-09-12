# Stage 1 Bootloader Documentation
## Overview
This repository contains the code for a multi-stage bootloader, with Stage 1 being responsible for loading Stage 2 into memory. This bootloader runs in Real Mode on x86 architecture, and initializes segment registers, sets up the stack, and reads sectors from the disk.

In this document, you will find an in-depth explanation of each section of the code.

## Table of Contents
- [Pre-Bootloader: System Power-On](#pre-bootloader-system-power-on)
- [Stage 1 Bootloader Overview](#stage-1-bootloader-overview)
- [Setting Up the Environment](#setting-up-the-environment)
- [Entry Point](#entry-point)
- [Segment Register Setup](#segment-register-setup)
- [Stack Setup](#stack-setup)
- [Loading Stage 2 Bootloader](#loading-stage-2-bootloader)
- [Error Handling](#error-handling)
- [Padding and Boot Signature](#padding-and-boot-signature)
- [Code Reference](#code-reference)
- [Bootloader Code Layout: Hard Disk (HD) and Physical Memory (PM) Mapping](#bootloader-code-layout-hard-disk-hd-and-physical-memory-pm-mapping)

---

## Pre-Bootloader: System Power-On
When the power button is pressed, an electronic signal is sent to the motherboard.
The **Power Supply Unit (PSU)** then sends a signal called ```power_good``` to the **Basic Input Output System 
(BIOS)**. Upon receiving this signal, the BIOS begins the **POST** (Power-On Self-Test) process.

The BIOS executes INT 0x19 to search for a bootable device. If a bootable device is found, it loads the boot sector at the address ```0x7C00```.

Our bootloader's Stage 1 is designed to be loaded at ```0x7C00```.

---

## Stage 1 Bootloader Overview
The **Stage 1 Bootloader** is a 512-byte program that executes in Real Mode. It:

- Initializes system segment registers.
- Sets up the stack.
- Loads the second stage of the bootloader from the disk.
- Transfers control to Stage 2.

---

## Setting Up the Environment
```
[BITS 16]              ; We are working in 16-bit Real Mode
[org 0x7C00]           ; Origin address in memory (0x7C00)
```
- [BITS 16]: Tells the assembler we are working in 16-bit Real Mode.
- [org 0x7C00]: The bootloader starts execution at address 0x7C00, which is where the BIOS loads it into memory.

---

## Entry Point
```
start:                ; Start of execution
    jmp main          ; Jump to the main label
```
- start: This is where the execution begins.
- jmp main: This instruction jumps to the main label to skip any initial data and begin the core setup of the bootloader.

---

## Segment Register Setup
```
main:                   
    cli                 ; Disable interrupts
    xor ax, ax          ; Clear AX register (sets AX to 0)
    mov ds, ax          ; Set Data Segment (DS) to 0x0
    mov es, ax          ; Set Extra Segment (ES) to 0x0
```
- cli: Clears interrupts to prevent any interruptions during setup.
- xor ax, ax: Clears the AX register by XORing it with itself (sets it to 0). In real mode, the physical address is calculated as Segment * 16 + Offset. Setting AX to 0 ensures that the segments point to the base of memory (0x0000).
- mov ds, ax: Sets the Data Segment (DS) to 0, making memory accesses relative to the base of memory.
- mov es, ax: Sets the Extra Segment (ES) to 0, allowing any extra memory accesses to also reference the base of memory.

---

## Stack Setup
```
    mov ss, ax          ; Set Stack Segment (SS) to 0 (base of memory).
    mov sp, 0xFFFE      ; Set Stack Pointer (SP) to the highest address within the current 64KB segment (0x0000:0xFFFE).
                        ; The value 0xFFFE avoids potential alignment issues.
    sti                 ; Re-enable interrupts after the setup is complete.
```
- mov ss, ax: Sets the Stack Segment (SS) to 0, so the stack will be placed at the base of memory.
- mov sp, 0xFFFE: Sets the Stack Pointer (SP) to 0xFFFE, meaning the stack will start near the highest address in the 64KB segment. This ensures that the stack grows downward from a safe 
  position. The value 0xFFFE is used to avoid alignment issues, as it's an even address.
- sti: Re-enables interrupts after the segment and stack setup is complete, allowing the system to handle interrupts again.

---

## Loading Stage 2 Bootloader
```
    mov ah, 02h         ; BIOS Interrupt 13h, Function 02h: Read sectors from the disk
    mov al, 01h         ; Read 1 sector
    mov ch, 00h         ; Cylinder 0
    mov cl, 02h         ; Sector 2
    mov dh, 00h         ; Head 0
    mov dl, 80h         ; Primary hard disk
    mov bx, 0x8000      ; Offset for loading Stage 2
    int 13h             ; Call BIOS to load the sector
    jc disk_read_error  ; Jump to error handler if read fails
```
- mov ah, 02h: Calls the BIOS interrupt 13h to read sectors from the disk.
- mov al, 01h: Requests to read one sector.
- mov ch, cl, dh, dl: These registers specify the disk geometry: Cylinder 0, Sector 2, Head 0, Disk 0x80 (primary hard disk).
- mov bx, 0x8000: Specifies the memory location 0x8000 where Stage 2 will be loaded. The physical address where Stage 2 is loaded is calculated as ES * 16 + BX, resulting in 0x8000.
- int 13h: Executes the disk read command using BIOS interrupt 13h.
- jc disk_read_error: If the disk read fails (carry flag is set), the code jumps to the error handler.

---

## Error Handling
```
disk_read_error:
    int 18h             ; Attempt a boot from a different device
```
- disk_read_error: If the disk read fails, this routine attempts to boot from an alternative device using BIOS Interrupt 18h.

---

## Padding and Boot Signature
```
TIMES 510-($-$$) DB 0   ; Pad to 512 bytes
DW 0xAA55               ; Boot signature
```
- TIMES 510-($-$$) DB 0: Fills the remaining space with zeros, padding the bootloader to exactly 512 bytes (BIOS requirement).
- DW 0xAA55: This is the boot signature required by the BIOS to identify the bootloader.

---

## Code Reference
Here’s the complete Stage 1 bootloader code:
```
[BITS 16]
[org 0x7c00]

start:
    jmp main

main:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax

    mov ss, ax
    mov sp, 0xFFFE

    sti

    mov ah, 02h
    mov al, 01h

    mov ch, 00h
    mov cl, 02h
    mov dh, 00h
    mov dl, 80h

    mov bx, 0x8000
    int 13h

    jc disk_read_error

pass:
    jmp 0x0000:0x8000

disk_read_error:
    int 18h

TIMES 510-($-$$) DB 0
DW 0xAA55          
```

---

## Bootloader Code Layout: Hard Disk (HD) and Physical Memory (PM) Mapping

### Hard Disk:

![Bootloader Layout](https://github.com/IlanVinograd/OS_32Bit/blob/main/Images/HD.png)

### Physical Address:

![Bootloader Layout](https://github.com/IlanVinograd/OS_32Bit/blob/main/Images/PA.png)
