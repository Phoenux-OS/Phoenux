#ifndef __GDT_H__
#define __GDT_H__

#include <stddef.h>

void load_gdt(void);
void set_segment(int entry, size_t base, size_t page_count);
void load_tss(void);

#endif
