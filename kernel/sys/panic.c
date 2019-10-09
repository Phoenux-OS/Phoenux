#include <lib/kprint.h>
#include <sys/trace.h>
#include <sys/panic.h>

void panic(const char *msg) {
    asm volatile ("cli");
    kprint(KPRN_ERR, "Kernel panic: %s", msg);
    print_stacktrace();
    asm volatile ("hlt");
}
