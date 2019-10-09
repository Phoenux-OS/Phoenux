bits 32

section .entry

global _start

extern kernel_init
extern bss_start
extern bss_end
_start:
    mov edi, bss_start
    mov ecx, bss_end
    sub ecx, bss_start
    xor al, al
    rep stosb
    mov esp, 0xeffff0
    xor ebp, ebp
    call kernel_init
  .die:
    cli
    hlt
    jmp .die
