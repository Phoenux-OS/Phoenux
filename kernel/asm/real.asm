global real_routine_

segment DATA class=DATA use32

%define real_init_size  real_init_end - real_init
real_init:              incbin "real/real_init.bin"
real_init_end:

segment CODE class=CODE use32

real_routine_:
    ; ESI = routine location
    ; ECX = routine size
    pusha
    push ds
    push es
    push fs
    push gs
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esi
    push ecx

    ; Real mode init blob to 0000:1000
    mov esi, real_init
    mov edi, 0x1000
    mov ecx, real_init_size
    rep movsb

    ; Routine's blob to 0000:8000
    pop ecx
    pop esi
    mov edi, 0x8000
    rep movsb

    ; Call module
    mov edi, 0x1000
    call edi

    pop gs
    pop fs
    pop es
    pop ds
    popa
    iret
