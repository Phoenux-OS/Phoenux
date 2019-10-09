#include <stddef.h>
#include <symlist.h>
#include <lib/kprint.h>
#include <sys/trace.h>

struct symlist_t *trace_address(size_t addr) {
    for (size_t i = 0; ; i++) {
        if (symlist[i].addr >= addr)
            return &symlist[i-1];
    }
}

void print_stacktrace(void) {
    size_t *base_ptr;
    asm volatile (
        "mov %0, ebp;"
        : "=r"(base_ptr)
    );
    kprint(KPRN_INFO, "Stacktrace:");
    for (;;) {
        size_t old_bp = base_ptr[0];
        size_t ret_addr = base_ptr[1];
        struct symlist_t *t = trace_address(ret_addr);
        kprint(KPRN_INFO, "  [%x] <%s>", t->addr, t->name);
        if (!old_bp)
            break;
        base_ptr = (void*)old_bp;
    }
}
