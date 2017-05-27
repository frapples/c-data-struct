#include "ccstr.h"

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include "alloc.h"

typedef struct {
    size_t size;
    char c_str[];
}ccstring_full_t;

#define FULL_STRUCT(p) ((ccstring_full_t*)((char *)p - offsetof(ccstring_full_t, c_str)))


ccstr_t ccstr_create(const char* c_str)
{
    size_t len = strlen(c_str);
    ccstring_full_t* full = fds_malloc(sizeof(ccstring_full_t) + len + 1);
    full->size = len;
    strcpy(full->c_str, c_str);
    return full->c_str;
}

void ccstr_destory(ccstr_t str)
{
    ccstring_full_t* full = FULL_STRUCT(str);
    fds_free(full);
}

ccstr_t ccstr_concat(const char* str1, ...)
{
    va_list ap;
    va_start(ap, str1);

    size_t len = strlen(str1);
    ccstring_full_t* result = fds_malloc(sizeof(ccstring_full_t) + len + 1);
    result->size = len;
    strcpy(result->c_str, str1);

    int debug_count = 0;

    const char* str = va_arg(ap, const char*);
    while (str != NULL) {
        size_t len = strlen(str);
        result = fds_realloc(result, sizeof(ccstring_full_t) + result->size + len + 1);
        strcpy(result->c_str + result->size, str);
        result->size += len;

        str = va_arg(ap, const char*);

#ifndef NDEBUG
        debug_count++;
        assert(debug_count < 16);
#endif // NDEBUG
    }

    va_end(ap);
    return result->c_str;
}

size_t ccstr_len(ccstr_t str)
{
    ccstring_full_t* full = FULL_STRUCT(str);
    return full->size;
}
