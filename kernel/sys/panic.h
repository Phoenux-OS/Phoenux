#ifndef __SYS__PANIC_H__
#define __SYS__PANIC_H__

#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>
#include <sys/cpu.h>

void panic(struct regs_t *regs, bool print_trace, const char *fmt, ...);

#endif
