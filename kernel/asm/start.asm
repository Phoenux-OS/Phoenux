extern kernel_init_

segment STRT class=STRT use32

..start:
    mov esp, 0xeefff0
    xor ebp, ebp
    call kernel_init_
  .die:
    cli
    hlt
    jmp .die
