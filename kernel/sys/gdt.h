#ifndef __SYS__GDT_H__
#define __SYS__GDT_H__

#include <stddef.h>

void load_gdt(void);
void set_segment(int entry, size_t base, size_t page_count);
void load_tss(void);

#endif
