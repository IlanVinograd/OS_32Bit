.section .entry, "ax"
.extern _start

_entry:
    call _start

hltloop:
    hlt
    jmp hltloop
