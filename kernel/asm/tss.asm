global tss
global tss_size

section .data

align 4
tss:
    dd 0
    dd 0xEFFFF0
    dd 0x10
    times 23 dd 0
tss_end:

tss_size equ tss_end - tss
