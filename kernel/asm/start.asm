extern kernel_init_

segment _STRT class=STRT use32

global _start
_start:
    mov esp, 0xeefff0
    xor ebp, ebp
    call kernel_init_
  .die:
    cli
    hlt
    jmp .die
