#ifndef __ALLOC_H__RAND217103883253425442
#define __ALLOC_H__RAND217103883253425442

#include <stddef.h>

typedef void* (*MallocFunc)(size_t);
typedef void (*FreeFunc)(void*);

extern MallocFunc malloc_function;
extern FreeFunc free_function;

#define NEW(type, size) MallocFunc(sizeof(type) * size)

#define FREE(p) free_function(p)

#endif /* __ALLOC_H__RAND217103883253425442 */
