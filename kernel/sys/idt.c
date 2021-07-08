#include <stddef.h>
#include <stdint.h>
#include <sys/idt.h>

static struct idt_entry_t idt[256];

void idt_load(void*);

void init_idt(void) {
    /* Register all interrupts */
/*
    for (size_t i = 0; i < 256; i++)
        register_interrupt_handler(i, invalid_interrupt, 0x8e);
*/
    /* Exception handlers */
    register_interrupt_handler(0x0, exc_div0_handler, 0x8e);
    register_interrupt_handler(0x1, exc_debug_handler, 0x8e);
    register_interrupt_handler(0x2, exc_nmi_handler, 0x8e);
    register_interrupt_handler(0x3, exc_breakpoint_handler, 0x8e);
    register_interrupt_handler(0x4, exc_overflow_handler, 0x8e);
    register_interrupt_handler(0x5, exc_bound_range_handler, 0x8e);
    register_interrupt_handler(0x6, exc_inv_opcode_handler, 0x8e);
    register_interrupt_handler(0x7, exc_no_dev_handler, 0x8e);
    register_interrupt_handler(0x8, exc_double_fault_handler, 0x8e);
    register_interrupt_handler(0xa, exc_inv_tss_handler, 0x8e);
    register_interrupt_handler(0xb, exc_no_segment_handler, 0x8e);
    register_interrupt_handler(0xc, exc_ss_fault_handler, 0x8e);
    register_interrupt_handler(0xd, exc_gpf_handler, 0x8e);
    register_interrupt_handler(0xe, exc_page_fault_handler, 0x8e);
    register_interrupt_handler(0x10, exc_x87_fp_handler, 0x8e);
    register_interrupt_handler(0x11, exc_alignment_check_handler, 0x8e);
    register_interrupt_handler(0x12, exc_machine_check_handler, 0x8e);
    register_interrupt_handler(0x13, exc_simd_fp_handler, 0x8e);
    register_interrupt_handler(0x14, exc_virt_handler, 0x8e);
    /* 0x15 .. 0x1d resv. */
    register_interrupt_handler(0x1e, exc_security_handler, 0x8e);

    struct idt_ptr_t idt_ptr = {
        sizeof(idt) - 1,
        (uint32_t)idt
    };

    idt_load(&idt_ptr);
}

void register_interrupt_handler(size_t vec, void (*handler)(void), uint8_t type) {
    uint32_t p = (uint32_t)handler;

    idt[vec].offset_lo = (uint16_t)p;
    idt[vec].selector = 0x08;
    idt[vec].unused = 0;
    idt[vec].type_attr = type;
    idt[vec].offset_hi = (uint16_t)(p >> 16);
}
