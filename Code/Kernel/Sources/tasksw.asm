extern current
extern tss
global switch_to_task

;typedef struct task {
;    uint32_t pid;               // Process ID
;    enum State state;           // Process state
;    uintptr_t pc;               // Program Counter
;    uintptr_t *sp;              // Stack Pointer
;    uintptr_t esp0;             // Stack Pointer
;    uintptr_t ss0;              // Stack Pointer
;    uint32_t flags;             // Status/Flags register
;    struct task *next;          // Pointer to the next task in the list
;} task;

; Locations must match the task structure in PCB.h
struc PCB
     .pid   resd 1
     .state resd 1
     .pc    resd 1
     .esp   resd 1
     .esp0  resd 1
     .ss0   resd 1
     .flags resd 1
     .next  resd 1
endstruc

;// TSS entry structure
;struct tss_entry {
;    uint32_t prev_tss;      // Previous TSS (if using hardware task switching)
;    uint32_t esp0;          // Stack pointer for ring 0
;    uint32_t ss0;           // Stack segment for ring 0
;    // Additional fields can be zero
;    uint32_t reserved[22];  // Reserved fields to make the structure size correct
;    uint16_t reserved1;     // Reserved
;    uint16_t iomap_base;    // IOMAP base
;}

struc TSS
     .prev_tss   resd 1
     .esp0       resd 1
     .ss0        resd 1
     .reserved   resd 22
     .reserved1  resw 1
     .iomap_base resw 1
endstruc

section .text

;C declaration:
;   void switch_to_task(task *next_task);
;
;WARNING: Caller is expected to disable IRQs before calling, and enable IRQs again after function returns

switch_to_task:

    ;Save previous task's state

    ;Notes:
    ;  For cdecl; EAX, ECX, and EDX are already saved by the caller and don't need to be saved again
    ;  EIP is already saved on the stack by the caller's "CALL" instruction
    ;  Segment registers are constants (while running kernel code) so they don't need to be saved

    ; Preserve the non-volatile registers (per the 32-bit System V ABI)
    push ebx
    push esi
    push edi
    push ebp

    mov edi,[current]             ;edi = address of the previous task's (current) "process control block"
    mov [edi+PCB.esp],esp         ;Save ESP for previous task's kernel stack in the process's PCB

    ; Load next task's state
    mov esi,[esp+(4+1)*4]         ;esi = address of the next task's "process control block" (parameter passed on stack)
    mov [current],esi             ;Current task's PCB is the next task PCB

    mov esp,[esi+PCB.esp]         ;Load ESP for next task's kernel stack from the process's PCB
    mov ebx,[esi+PCB.esp0]        ;ebx = address for the top of the next task's kernel stack

    ; TSS SS0:ESP0 apply to user mode tasks and can be removed if using just kernel mode tasks
    mov [tss+TSS.esp0],ebx        ;Adjust the ESP0 field in the TSS (used by CPU for for CPL=3 -> CPL=0 privilege level changes)
    mov ebx,[esi+PCB.ss0]         ;ebx = segment for the kernel stack
    mov dword [tss+TSS.ss0], ebx  ;Adjust the SS0 field in the TSS (used by CPU for for CPL=3 -> CPL=0 privilege level changes)

    ; Restore the non-volatile registers (per the 32-bit System V ABI)
    pop ebp
    pop edi
    pop esi
    pop ebx

    ret                           ;Load next task's EIP from its kernel stack
