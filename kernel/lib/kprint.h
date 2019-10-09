#ifndef __LIB__KPRINT_H__
#define __LIB__KPRINT_H__

#include <stdarg.h>

#define KPRN_INFO   0
#define KPRN_WARN   1
#define KPRN_ERR    2
#define KPRN_DBG    3
#define KPRN_PANIC  4

void kprint(int type, const char *fmt, ...);
void kvprint(int type, const char *fmt, va_list args);

#endif
