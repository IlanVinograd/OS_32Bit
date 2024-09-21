[bits 32]
extern kernel_main
global _start

_start:
    call kernel_main    ; Call the C kernel entry
    hlt                 ; Halt the system if kernel_main returns
