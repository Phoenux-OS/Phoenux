global tss
global tss_size

section .data

align 4
tss:
    dd 0
    dd 0xEEFFF0
    dd 0x10
    dd 0xEFFFF0
    dd 0x21
    times 21 dd 0
tss_end:

tss_size equ tss_end - tss
