#ifndef __ALLOC_H__RAND217103883253425442
#define __ALLOC_H__RAND217103883253425442

#include <stddef.h>

typedef void* (*MallocFunc)(size_t);
typedef void (*FreeFunc)(void*);
typedef void* (*ReallocFunc)(void*, size_t);

#define FDS_NEW(type, size) fds_malloc(sizeof(type) * size)

#endif /* __ALLOC_H__RAND217103883253425442 */
