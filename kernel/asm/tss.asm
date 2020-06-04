global _tss
global _tss_size

segment _DATA class=DATA use32

align 4
_tss:
    dd 0
    dd 0xEFFFF0
    dd 0x10
    dd 0xEEFFF0
    dd 0x21
    times 21 dd 0
_tss_end:

_tss_size equ _tss_end - _tss
