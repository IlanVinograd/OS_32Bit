section .text
global switch_to_task_handler

switch_to_task_handler:
    cli                       ; Disable interrupts
    push ebx
    push esi
    push edi
    push ebp

    ; Save current task's stack pointer
    mov [esi + 12], esp       ; Store ESP into the current task's PCB

    ; Load next task's stack pointer
    mov esp, [edi + 12]       ; Load ESP from the next task's PCB

    ; Restore callee-saved registers
    pop ebp
    pop edi
    pop esi
    pop ebx

    sti                       ; Enable interrupts
    jmp [edi + 8]             ; Load EIP from the next task's PCB and jump to it