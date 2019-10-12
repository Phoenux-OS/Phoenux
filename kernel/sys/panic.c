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
        kprint(KPRN_PANIC, "  EAX: %8x  EBX: %8x  ECX: %8x  EDX: %8x",
                           regs->eax,
                           regs->ebx,
                           regs->ecx,
                           regs->edx);
        kprint(KPRN_PANIC, "  ESI: %8x  EDI: %8x  EBP: %8x  ESP: %8x",
                           regs->esi,
                           regs->edi,
                           regs->ebp,
                           regs->esp);
        kprint(KPRN_PANIC, "  EIP: %8x  EFLAGS: %8x", regs->eip, regs->eflags);
        kprint(KPRN_PANIC, "  CS:  %4x  SS: %4x", regs->cs, regs->ss);
    }

    if (print_trace)
        print_stacktrace(KPRN_PANIC);

    asm volatile ("hlt");
}
