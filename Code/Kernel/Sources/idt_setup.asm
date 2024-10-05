[BITS 32]

extern idt_p           ; IDT pointer
extern isr0_handler    ; C function to handle ISR0 (Divide by Zero)
extern isr6_handler    ; C function to handle ISR6 (Invalid Opcode)
extern isr13_handler    ; C function to handle ISR13 (General Protection Fault)
extern isr14_handler    ; C function to handle ISR14 (Page Fault)

global load_idt
global isr0
global isr6
global isr13
global isr14

section .text

; Function to load the IDT
load_idt:
    lidt [idt_p]       ; Load IDT pointer into the IDTR register
    ret

; ISR for Divide by Zero Exception (ISR0)
isr0:
    cli                ; Disable interrupts
    pusha              ; Save all registers

    call isr0_handler  ; Call the C function to print the exception message

    popa               ; Restore registers
    out 0x20, al       ; Send End of Interrupt (EOI) to the PIC

    hlt                ; Halt the CPU to stop further execution
    jmp $              ; Infinite loop to halt system

; ISR for Invalid Opcode Exception (ISR6)
isr6:
    cli                ; Disable interrupts
    pusha              ; Save all registers

    call isr6_handler  ; Call the C function to print the Invalid Opcode message

    popa               ; Restore registers
    out 0x20, al       ; Send End of Interrupt (EOI) to the PIC

    hlt                ; Halt the CPU to stop further execution
    jmp $              ; Infinite loop to halt system

; ISR for General Protection Fault Exception (ISR13)
isr13:
    cli                ; Disable interrupts
    pusha              ; Save all registers

    call isr13_handler ; Call the C function to print the General Protection Fault message

    popa               ; Restore registers
    out 0x20, al       ; Send End of Interrupt (EOI) to the PIC

    hlt                ; Halt the CPU to stop further execution
    jmp $              ; Infinite loop to halt system

; ISR for Page Fault Exception (ISR14)
isr14:
    cli                ; Disable interrupts
    pusha              ; Save all registers

    call isr14_handler ; Call the C function to print the Page Fault message

    popa               ; Restore registers
    out 0x20, al       ; Send End of Interrupt (EOI) to the PIC

    hlt                ; Halt the CPU to stop further execution
    jmp $              ; Infinite loop to halt system