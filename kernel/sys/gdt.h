#ifndef __GDT_H__
#define __GDT_H__

#include <stdint.h>

void load_GDT(void);
void set_segment(uint16_t entry, uint32_t base, uint32_t page_count);
void load_TSS(void);

#endif
