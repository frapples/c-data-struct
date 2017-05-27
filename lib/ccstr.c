#include "ccstr.h"

#include <stdlib.h>

typedef struct {
    size_t size;
    char c_str[];
}ccstring_full_t;

#define FULL_STRUCT(p) ((ccstring_full_t*)((char *)p - offsetof(ccstring_full_t, c_str)))
