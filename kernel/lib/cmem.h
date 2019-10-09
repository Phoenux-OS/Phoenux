#ifndef __LIB__CMEM_H__
#define __LIB__CMEM_H__

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t count);
void *memset(void *s, int c, size_t count);
void *memmove(void *dest, const void *src, size_t count);
int memcmp(const void *s1, const void *s2, size_t n);

#endif
