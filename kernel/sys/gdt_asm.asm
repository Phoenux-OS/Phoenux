segment CODE class=CODE use32

global gdt_load_
gdt_load_:
    lgdt [eax]
    mov eax, .ret
    push 0x08
    push eax
    retf
  .ret:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ret

global go_to_ring1_
go_to_ring1_:
    mov ax, 0x48
    ltr ax
    mov eax, esp
    push 0x21
    push eax
    pushf
    pop eax
    or eax, 1 << 12
    push eax
    push 0x19
    mov eax, .ret
    push eax
    iret
  .ret:
    mov ax, 0x21
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    ret
