global get_e820_

segment DATA class=DATA use32

%define e820_size           e820_end - e820_bin
e820_bin:                   incbin "real/e820.bin"
e820_end:

segment CODE class=CODE use32

get_e820_:
    ; void get_e820(e820_entry_t *e820_map);
    push ebx
    push esi
    push ecx

    mov ebx, eax
    mov esi, e820_bin
    mov ecx, e820_size
    int 0x48

    pop ecx
    pop esi
    pop ebx
    ret
