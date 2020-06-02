; Exception handler_s.
global exc_div0_handler_
global exc_debug_handler_
global exc_nmi_handler_
global exc_breakpoint_handler_
global exc_overflow_handler_
global exc_bound_range_handler_
global exc_inv_opcode_handler_
global exc_no_dev_handler_
global exc_double_fault_handler_
global exc_inv_tss_handler_
global exc_no_segment_handler_
global exc_ss_fault_handler_
global exc_gpf_handler_
global exc_page_fault_handler_
global exc_x87_fp_handler_
global exc_alignment_check_handler_
global exc_machine_check_handler_
global exc_simd_fp_handler_
global exc_virt_handler_
global exc_security_handler_

extern exception_handler_

%macro interrupt_enter 0
    cld
    push eax
    push ebx
    push ecx
    push edx
    push esi
    push edi
    push ebp
    push ds
    push es
    push fs
    push gs
    push 0x10
    push 0x10
    push 0x10
    push 0x10
    pop ds
    pop es
    pop fs
    pop gs
%endmacro

%macro interrupt_leave 0
    pop gs
    pop fs
    pop es
    pop ds
    pop ebp
    pop edi
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
%endmacro

%macro except_handler_err_code 1
    push dword [esp+5*4]
    push dword [esp+5*4]
    push dword [esp+5*4]
    push dword [esp+5*4]
    push dword [esp+5*4]
    interrupt_enter
    mov eax, esp
    push dword [esp+20*4]
    push eax
    push %1
    call exception_handler_
    interrupt_leave
    iret
%endmacro

%macro except_handler 1
    interrupt_enter
    mov eax, esp
    push 0
    push eax
    push %1
    call exception_handler_
    interrupt_leave
    iret
%endmacro

segment CODE class=CODE use32

; Exception handler_s
exc_div0_handler_:
    except_handler 0x0
exc_debug_handler_:
    except_handler 0x1
exc_nmi_handler_:
    except_handler 0x2
exc_breakpoint_handler_:
    except_handler 0x3
exc_overflow_handler_:
    except_handler 0x4
exc_bound_range_handler_:
    except_handler 0x5
exc_inv_opcode_handler_:
    except_handler 0x6
exc_no_dev_handler_:
    except_handler 0x7
exc_double_fault_handler_:
    except_handler_err_code 0x8
exc_inv_tss_handler_:
    except_handler_err_code 0xa
exc_no_segment_handler_:
    except_handler_err_code 0xb
exc_ss_fault_handler_:
    except_handler_err_code 0xc
exc_gpf_handler_:
    except_handler_err_code 0xd
exc_page_fault_handler_:
    except_handler_err_code 0xe
exc_x87_fp_handler_:
    except_handler 0x10
exc_alignment_check_handler_:
    except_handler_err_code 0x11
exc_machine_check_handler_:
    except_handler 0x12
exc_simd_fp_handler_:
    except_handler 0x13
exc_virt_handler_:
    except_handler 0x14
exc_security_handler_:
    except_handler_err_code 0x1e
