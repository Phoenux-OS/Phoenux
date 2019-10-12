#ifndef __SYS__PANIC_H__
#define __SYS__PANIC_H__

#include <stddef.h>
#include <stdarg.h>
#include <sys/cpu.h>

void panic(struct regs_t *regs, const char *fmt, ...);

#endif
