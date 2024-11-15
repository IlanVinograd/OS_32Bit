global switch_to_task_handler

section .text
switch_to_task_handler:
    pusha
    mov [esi + 12], esp
    mov esp, [edi + 12]
    popa
    ret