segment CODE class=CODE use32

global bit_test_
bit_test_:
    bt eax, edx
    xor eax, eax
    setc al
    ret

global test_bit_
test_bit_:
    bt [eax], edx
    xor eax, eax
    setc al
    ret

global set_bit_
set_bit_:
    bts [eax], edx
    xor eax, eax
    setc al
    ret

global reset_bit_
reset_bit_:
    btr [eax], edx
    xor eax, eax
    setc al
    ret
