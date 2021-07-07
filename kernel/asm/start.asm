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
    dq stack.top
    dw 0
    dw 0
    dw 0
    dw 0
    dq kernel_init_

align 16
stivale_anchor:
    db 'STIVALE1 ANCHOR'
    db 32
    dq 0x200000
    dq _edata
    dq _end
    dq stivalehdr
