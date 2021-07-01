extern kernel_init_
extern _edata
extern _end

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

align 16
multiboot_header:
    .magic dd 0x1badb002
    .flags dd 0x00010000
    .checksum dd -(0x1badb002 + 0x00010000)
    .header_addr dd multiboot_header
    .load_addr dd 0x200000
    .load_end_addr dd _edata
    .bss_end_addr dd _end
    .entry_addr dd _start
