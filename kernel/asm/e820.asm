extern real_routine

global get_e820

section .data

%define e820_size           e820_end - e820_bin
e820_bin:                   incbin "real/e820.bin"
e820_end:

section .text

bits 32

get_e820:
    ; void get_e820(e820_entry_t *e820_map);
    push ebx
    push esi
    push ecx

    mov ebx, dword [esp+16]
    mov esi, e820_bin
    mov ecx, e820_size
    call real_routine

    pop ecx
    pop esi
    pop ebx
    ret
