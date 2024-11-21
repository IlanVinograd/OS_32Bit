[BITS 32]

extern isr0_handler    ; C function to handle ISR0 (Divide by Zero)
extern isr6_handler    ; C function to handle ISR6 (Invalid Opcode)
extern isr13_handler    ; C function to handle ISR13 (General Protection Fault)
extern isr14_handler    ; C function to handle ISR14 (Page Fault)

extern pit_handler

extern irq0_handler
extern irq1_handler 

global load_idt
global isr0
global isr6
global isr13
global isr14
global isr32

global irq0
global irq1

section .text

; No need for CLI as interrupt gates have interrupts turned off
; by the CPU before they start executing a handler.

; ISR for Divide by Zero Exception (ISR0)
isr0:
    pusha              ; Save all registers

    cld                ; DF = 0 required by 32-bit System V ABI
    call isr0_handler  ; Call the C function to print the exception message

    popa               ; Restore registers

    hlt                ; Halt the CPU to stop further execution

; ISR for Invalid Opcode Exception (ISR6)
isr6:
    pusha              ; Save all registers

    cld                ; DF = 0 required by 32-bit System V ABI
    call isr6_handler  ; Call the C function to print the Invalid Opcode message

    popa               ; Restore registers

    hlt                ; Halt the CPU to stop further execution

; ISR for General Protection Fault Exception (ISR13)
isr13:
    pusha              ; Save all registers

    cld                ; DF = 0 required by 32-bit System V ABI
    call isr13_handler ; Call the C function to print the General Protection Fault message

    popa               ; Restore registers

    hlt                ; Halt the CPU to stop further execution

; ISR for Page Fault Exception (ISR14)
isr14:
    pusha              ; Save all registers

    cld                ; DF = 0 required by 32-bit System V ABI
    call isr14_handler ; Call the C function to print the Page Fault message

    popa               ; Restore registers

    add esp, 4         ; Remove error code
    iret

; PIT IRQ0 - Timer
irq0:
    pusha               ; Save all registers
    call pit_handler    ; Call the PIT handler to increment tick_count
    popa                ; Restore registers
    iret

; IRQ1 - Keyboard
irq1:
    pusha               ; Save all registers
    call irq1_handler   ; Call the C handler function for IRQ1
    popa                ; Restore registers
    iret