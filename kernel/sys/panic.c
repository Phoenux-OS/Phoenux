#include <stdarg.h>
#include <stdbool.h>
#include <lib/kprint.h>
#include <sys/trace.h>
#include <sys/panic.h>

void panic(struct regs_t *regs, bool print_trace, const char *fmt, ...) {
    asm volatile ("cli");

    va_list args;
    va_start(args, fmt);
    kprint(KPRN_PANIC, "Kernel panic:");
    kvprint(KPRN_PANIC, fmt, args);
    va_end(args);

    if (regs) {
        kprint(KPRN_PANIC, "CPU status at fault:");
        kprint(KPRN_PANIC, "  EAX:    %x", regs->eax);
        kprint(KPRN_PANIC, "  EBX:    %x", regs->ebx);
        kprint(KPRN_PANIC, "  ECX:    %x", regs->ecx);
        kprint(KPRN_PANIC, "  EDX:    %x", regs->edx);
        kprint(KPRN_PANIC, "  ESI:    %x", regs->esi);
        kprint(KPRN_PANIC, "  EDI:    %x", regs->edi);
        kprint(KPRN_PANIC, "  EBP:    %x", regs->ebp);
        kprint(KPRN_PANIC, "  ESP:    %x", regs->esp);
        kprint(KPRN_PANIC, "  EFLAGS: %x", regs->eflags);
        kprint(KPRN_PANIC, "  EIP:    %x", regs->eip);
        kprint(KPRN_PANIC, "  CS:     %x", regs->cs);
        kprint(KPRN_PANIC, "  SS:     %x", regs->ss);
    }

    if (print_trace)
        print_stacktrace(KPRN_PANIC);

    asm volatile ("hlt");
}
