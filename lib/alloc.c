#include "alloc.h"

#include <stdio.h>
#include <stdlib.h>

static MallocFunc malloc_function = malloc;
static FreeFunc free_function = free;
static ReallocFunc realloc_function = realloc;

/* malloc和free的次数不成对时拒绝修改allocator */
static int count = 0;

void* fds_malloc(size_t size)
{
    count++;
    return malloc_function(size);
}

void* fds_realloc(void* p, size_t size)
{
    return realloc_function(p, size);
}

void fds_free(void* p)
{
    if (p != NULL) {
        count--;
        free_function(p);
    }
}

void fds_set_allocator(MallocFunc alloc_func, FreeFunc free_func)
{
    if (count != 0) {
        fprintf(stderr, "内存未释放干净，拒绝修改allocator\n");
    }

    malloc_function = alloc_func;
    free_function = free_func;
}

