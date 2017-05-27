#ifndef __ALLOC_H__RAND217103883253425442
#define __ALLOC_H__RAND217103883253425442

#include <stddef.h>

typedef void* (*MallocFunc)(size_t);
typedef void (*FreeFunc)(void*);
typedef void* (*ReallocFunc)(void*, size_t);

void* fds_malloc(size_t size);
void* fds_realloc(void* p, size_t size);
void fds_free(void* p);
void fds_set_allocator(MallocFunc alloc_func, FreeFunc free_func);

#define FDS_NEW(type, size) fds_malloc(sizeof(type) * size)



#endif /* __ALLOC_H__RAND217103883253425442 */
