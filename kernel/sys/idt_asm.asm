segment _TEXT class=CODE use32

global idt_load_
idt_load_:
    lidt [eax]
    ret
