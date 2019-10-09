#include <stddef.h>
#include <symlist.h>
#include <lib/kprint.h>
#include <sys/trace.h>

char *trace_address(size_t *off, size_t addr) {
    for (size_t i = 0; ; i++) {
        if (symlist[i].addr >= addr) {
            *off = addr - symlist[i-1].addr;
            return symlist[i-1].name;
        }
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
        size_t off;
        char *name = trace_address(&off, ret_addr);
        kprint(KPRN_INFO, "  [%x] <%s+%x>", ret_addr, name, off);
        if (!old_bp)
            break;
        base_ptr = (void*)old_bp;
    }
}
