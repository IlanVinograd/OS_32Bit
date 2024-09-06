## Bootloaders in-depth guide 
- ### ON POWER BUTTON (Pre bootloader)
  - When this button is pressed, the wires connected to the button send an electronic signal to the motherboard. The motherboard simply reroutes this signal to 
 the power supply (PSU).
 The PSU then sends a signal, called the "power_good" signal into the motherboard to the Basic Input Output System (BIOS).   
  - BIOS recieves this "power_good" signal, the BIOS begins initializing a process called POST.   
  The POST then gives control to the BIOS. The POST loads the BIOS at the end of memory (Might be 0xFFFFF0) and puts a jump instruction at the first byte in memory.     
  BIOS will execute Interrupt (INT) 0x19 to attempt to find a bootable device.   
  If the BIOS found a bootable device it load what is on `0x7c00`
  
  - Our bootloader is multi staged, this means we need to call STAGE1 first.    
  To do so we put STAGE1 at `0x7c00` using `[org 0x7c00]` and when the BIOS jumps to `0x7c00` it should find our own STAGE 1!

- ### STAGE 1 bootloader
  - Overview
This bootloader code is designed to run on x86 processors in real mode. It initializes system segments, sets up the stack, loads a secondary bootloader from disk, and then transfers control to it. Each sector of the code is explained in detail below.

- Setting Up the Environment
```
[BITS 16]               ; Specifies 16-bit real mode
[org 0x7c00]            ; Origin address where BIOS loads the bootloader (0x7C00)
```
**[BITS 16]** This directive tells the assembler that the code will be executed in 16-bit real mode. This mode is used during the initial boot phase of x86 processors, where memory is segmented into 64KB segments.

**[org 0x7c00]** The origin address of 0x7C00 is where the BIOS loads the bootloader into memory. This address is chosen by the BIOS and is where the bootloader begins execution.

- Entry Point
  ```
  start:                  ; Entry point of the bootloader
    jmp main            ; Jump to the 'main' routine
  ```
  **start:** This label marks the entry point of the bootloader. Execution begins here.

**jmp main** The jmp instruction skips over any potential data sections and jumps to the main routine where the primary setup and operations occur.

- Segment Register Setup
  ```
  main:                   ; Main routine of the bootloader

    cli                 ; Clear interrupts
    mov ax, 0x7C0       ; Load AX with segment base (0x7C00 >> 4)
    mov ds, ax          ; Set Data Segment to 0x7C0
    mov es, ax          ; Set Extra Segment to 0x7C0
    mov fs, ax          ; Set FS to 0x7C0
    mov gs, ax          ; Set GS to 0x7C0
  ```
  **cli** Clears interrupts to prevent any interruptions during the setup process. This ensures a stable environment for configuring segment registers.

  **mov ax, 0x7C0** Loads the AX register with 0x7C0, which is the segment base address. In real mode, the physical address is calculated as Segment * 16 + Offset.

  **mov ds, ax, mov es, ax, mov fs, ax, mov gs, ax** Sets the DS, ES, FS, and GS segment registers to 0x7C0. These registers point to the bootloader's memory segment.

- Stack Setup
```
      xor ax, ax          ; Clear AX register
    mov ss, ax          ; Set Stack Segment to 0
    mov sp, 0xFFFF      ; Set Stack Pointer to 0xFFFF
    sti                 ; Enable interrupts
 ```
  **xor ax, ax** Clears the AX register by XORing it with itself. This is a quick way to set AX to 0.

  **mov ss, ax** Sets the SS (Stack Segment) to 0. This points the stack segment to the base of the memory.

  **mov sp, 0xFFFF** Sets the SP (Stack Pointer) to 0xFFFF, which is the top of the 64KB memory block. The stack grows downward from this point.

  **sti** Re-enables interrupts after the stack setup is complete.

- Loading Stage 2 Bootloader
```
    mov ah, 02h         ; BIOS Interrupt 13h, Function 02h: Read sectors from disk
    mov al, 63h         ; Number of sectors to read (63 sectors for Stage 2)
    mov ch, 01h         ; Cylinder number (Stage 2 starts at Cylinder 1)
    mov cl, 01h         ; Sector number (First sector on Cylinder 1)
    mov dh, 00h         ; Head number (Assuming Head 0)
    mov es, ax          ; Destination segment for Stage 2
    mov bx, 0x8000      ; Destination address for Stage 2
    int 13h             ; Call BIOS interrupt 13h to read sectors
```
  **mov ah, 02h** Configures the BIOS interrupt 13h, function 02h to read sectors from the disk. This function is used to read data from the disk into memory.

  **mov al, 63h** Specifies the number of sectors to read (63 sectors). The number must match the size of Stage 2 to ensure the complete code is read.

  **mov ch, 01h, mov cl, 01h, mov dh, 00h** Sets the disk parameters—cylinder (CH), sector (CL), and head (DH). For this bootloader, Stage 2 starts on Cylinder 1, Sector 1, and Head 0.

**mov es, ax, mov bx, 0x8000** Sets the destination segment and address where Stage 2 will be loaded. Stage 2 will be read into the address 0x8000, which is computed as 0x08000 physical address.

**int 13h** Executes the disk read operation using BIOS interrupt 13h.

- Handling Disk Read Errors
```
    jc disk_read_error  ; If carry flag is set (error), jump to error handler
```
**jc disk_read_error** Checks the carry flag (CF). If it is set, indicating an error occurred during the disk read operation, the code jumps to the disk_read_error label for error handling.

- Successful Transfer to Stage 2
```
pass:                   ; Successful disk read
    jmp 0x0800:0x0000   ; Jump to Stage 2 bootloader
```
  **pass** Label indicating the successful completion of the disk read operation.

  **jmp 0x0800:0x0000** Jumps to the address where Stage 2 is loaded. The physical address is 0x0800 * 16 + 0x0000 = 0x8000.

  - Disk Read Error Handling
```
disk_read_error:
    int 18h             ; Attempt a boot from another device if disk read fails
```
**disk_read_error** Label for error handling when the disk read fails.

**int 18h** Calls BIOS interrupt 18h to attempt a boot from an alternative device, such as network boot, if the primary disk read fails.

- Padding and Boot Signature
```
TIMES 510-($-$$) DB 0   ; Pad the bootloader to 512 bytes with zeros
DW 0xAA55               ; Boot signature (magic number)
```
**TIMES 510-($-$$) DB 0** Pads the bootloader to exactly 512 bytes. This padding ensures that the bootloader meets the BIOS requirement of a 512-byte boot sector.

**DW 0xAA55** The boot signature, 0xAA55, is a magic number that the BIOS looks for to verify that the sector is a valid bootloader. This value must be present at the end of the boot sector.


                          Stage 1 of the bootloader, represented by -> #, is located at the CHS address Cylinder 0, Head 0, Sector 1 on the disk.
                          Stage 2, represented by -> @, is stored at Cylinder 1, Head 0, Sector 1.
                                
                                 | |
                                \   /
                                  V
                      ,,ggddY""""Ybbgg,,
                 ,agd"        |#######| ""bg,
              ,gdP"    8dyyt88|d88d8d|88     "Ybg,
            ,dP"    88        |@@@@@|   8Y8      "Yb,
          ,dP"   8     _,,ddP"|"Ybb|,_     8 8      "Yb,
         ,8"   8     ,dP"'    |   |  `"Yb,    8      "8,
        ,8'  8     ,d"        |dd|       "b,    8     `8,
       ,8'  8     d"          | |          "b    8   `8,
       d'  8     d'        ,gPPRg,        `b      8   `b
       8   8     8        dP'   `Yb        8      8    8
       8   8     8        8)  O  (8        8      8    8
       8   8     8        Yb     dP        8      8    8
       8   8     Y,        "8ggg8"        ,P     8    8
       Y,   8     Ya                     aP     8    ,P
       `8,   8      "Ya                 aP"     8    ,8'
        `8,  d8      "Yb,_         _,dP"      8    ,8'
         `8a     66   `""YbbgggddP""'      8     a8'
          `Yba     6                   8 8     adP'
            "Yba     6  6               d8      adY"
              `"Yba,     8d8d8d8d8d88d8   ,adP"'
                 `"Y8ba,             ,ad8P"'
                      ``""YYbaaadPP""''












