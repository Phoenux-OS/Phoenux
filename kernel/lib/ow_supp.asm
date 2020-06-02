segment CODE class=CODE use32

global __U8D
__U8D:
    div ebx
    mov ebx, edx
    xor edx, edx
    xor ecx, ecx
    ret

global __I8D
__I8D:
    idiv ebx
    mov ebx, edx
    xor edx, edx
    xor ecx, ecx
    ret

global __U8M
__U8M:
    mul ebx
    ret
