#ifndef __MM__PMM_H__
#define __MM__PMM_H__

#include <stddef.h>

#define PAGE_SIZE ((size_t)4096)

void *pmm_alloc(size_t);
void *pmm_allocz(size_t);
void pmm_free(void *, size_t);
void init_pmm(void);

#endif
