#include "arraylist.h"

#include <string.h>
#include <assert.h>

#include "alloc.h"

arraylist_t* arraylist_create(size_t item_size)
{
    arraylist_t* list = FDS_NEW(arraylist_t, 1);
    list->data = fds_malloc(0);
    list->len = 0;
    list->size = 0;
    list->item_size = item_size;

#ifndef NDEBUG
    list->debug_size2 = list->size;
    list->debug_len2 = list->len;
#endif

    return list;
}

arraylist_t* arraylist_append(arraylist_t* list, void* p_data)
{
    assert(list->size == list->debug_size2);
    assert(list->len == list->debug_len2);

    size_t size = (list->len + 1) * list->item_size;
    if (list->size < size) {
        list->data = fds_realloc(list->data, size);
        list->size = size;
    }
    memcpy(list->data + list->len * list->item_size, p_data, list->item_size);
    list->len++;

#ifndef NDEBUG
    list->debug_size2 = list->size;
    list->debug_len2 = list->len;
#endif

    return list;
}

void* arraylist_get(arraylist_t* list, size_t i)
{
    assert(i < list->len);

    return list->data + i * list->item_size;
}

size_t arraylist_len(arraylist_t* list)
{
    return list->len;
}

void arraylist_remove(arraylist_t* list, size_t start, size_t end)
{
    assert(start <= end);
    assert(start < list->len);
    assert(end <= list->len);

    for (size_t i = end; i < list->len; i++) {
        memcpy(arraylist_get(list, start + (i - end)), arraylist_get(list, i), list->item_size);
    }

    if (start < end) {
        list->len -= end - start;
    }


#ifndef NDEBUG
    list->debug_size2 = list->size;
    list->debug_len2 = list->len;
#endif
}

void arraylist_destory(arraylist_t* list)
{
    fds_free(list->data);
    fds_free(list);
}
