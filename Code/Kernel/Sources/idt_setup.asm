[BITS 32]

extern idt_p  ; IDT pointer
extern isr0_handler  ; Declare external C function

global load_idt
global isr0

section .text

; Function to load the IDT
load_idt:
    lidt [idt_p]  ; Load IDT pointer into the IDTR register
    ret

; ISR for Divide by Zero Exception (ISR0)
isr0:
    cli           ; Disable interrupts
    pusha         ; Save all registers

    call isr0_handler  ; Call the C function to print the exception message

    popa          ; Restore registers
    out 0x20, al  ; Send End of Interrupt (EOI) to the PIC

    hlt           ; Halt the CPU to stop further execution
    jmp $         ; Infinite loop to halt system

    sti           ; Enable interrupts (not necessary after halt)
    iret          ; Return from interrupt (not reached due to halt)