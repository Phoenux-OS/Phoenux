extern kernel_init_
extern _edata
extern _end

segment _BSS class=BSS use32

stack:
    resb 8192
.top:

segment _DATA class=DATA use32

align 16
stivalehdr:
    dd stack.top
    dd 0
    dw 0
    dw 0
    dw 0
    dw 0
    dd kernel_init_
    dd 0

align 16
stivale_anchor:
    db 'STIVALE1 ANCHOR'
    db 32
    dq 0x200000
    dd _edata
    dd 0
    dd _end
    dd 0
    dd stivalehdr
    dd 0
