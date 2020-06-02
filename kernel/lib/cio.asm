segment CODE class=CODE use32

global port_out_b_
port_out_b_:
    xchg eax, edx
    out dx, al
    ret

global port_out_w_
port_out_w_:
    xchg eax, edx
    out dx, ax
    ret

global port_out_d_
port_out_d_:
    xchg eax, edx
    out dx, eax
    ret

global port_in_b_
port_in_b_:
    mov edx, eax
    xor eax, eax
    in al, dx
    ret

global port_in_w_
port_in_w_:
    mov edx, eax
    xor eax, eax
    in ax, dx
    ret

global port_in_d_
port_in_d_:
    mov edx, eax
    in eax, dx
    ret
