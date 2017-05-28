#ifndef __ARRAYLIST_H__RAND19089429066720248
#define __ARRAYLIST_H__RAND19089429066720248

#include <stddef.h>
#include <assert.h>
#include "alloc.h"
/* 动态数组 */


typedef struct {
    size_t len;
    size_t size;
    size_t item_size;
#ifndef NDEBUG
    size_t debug_len2;
    size_t debug_size2;
#endif
    char* data;

}arraylist_t;

#define ARRAYLIST_GET(list, i, type) ((type*)arraylist_get_macro_utils(list, i, sizeof(type)))
void* arraylist_get(arraylist_t* list, size_t i);

arraylist_t* arraylist_create(size_t item_size);
void arraylist_destory(arraylist_t* list);
arraylist_t* arraylist_append(arraylist_t* list, void* p_data);
size_t arraylist_len(arraylist_t* list);
void arraylist_remove(arraylist_t* list, size_t start, size_t end);



static inline void* arraylist_get_macro_utils(arraylist_t* list, size_t i, size_t item_size)
{
    assert(item_size == list->item_size);

    return arraylist_get(list, i);
}


#endif /* __ARRAYLIST_H__RAND19089429066720248 */
