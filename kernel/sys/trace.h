#ifndef __SYS__TRACE_H__
#define __SYS__TRACE_H__

#include <stddef.h>
#include <symlist.h>

struct symlist_t *trace_address(size_t addr);
void print_stacktrace(void);

#endif
