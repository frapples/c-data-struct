#include "priorityqueue.h"
#include "arraylist.h"
#include "alloc.h"

#include <assert.h>

#define ROOT_INDEX 0
#define LEFT_INDEX(p) (2 * (p) + 1)
#define RIGHT_INDEX(p) (2 * (p) + 2)
#define PARENT_INDEX(n) ((n - 1) / 2)

priority_queue_t* priority_queue_create(CmpFunc cmpfunc) {
    priority_queue_t* queue = FDS_NEW(priority_queue_t, 1);
    queue->array = arraylist_create(sizeof(void *));
    queue->cmp_function = cmpfunc;
    return queue;
}

void priority_queue_insert(priority_queue_t* queue, void* data)
{
    arraylist_append(queue->array, &data);

    size_t i = arraylist_len(queue->array) - 1;
    while (i > 0) {
        int cmp = queue->cmp_function(data, *ARRAYLIST_GET(queue->array, PARENT_INDEX(i), void *));
        if (cmp < 0) {
            *ARRAYLIST_GET(queue->array, i, void *) = *ARRAYLIST_GET(queue->array, PARENT_INDEX(i), void *);
        } else {
            break;
        }
        i = PARENT_INDEX(i);
    }

    *ARRAYLIST_GET(queue->array, i, void *) = data;
}

void* priority_queue_delete_min(priority_queue_t* queue)
{
    assert(priority_queue_len(queue) > 0);

    void* min = *ARRAYLIST_GET(queue->array, 0, void *);
    size_t len = arraylist_len(queue->array);
    void* last = *ARRAYLIST_GET(queue->array, len - 1, void *);

    size_t i = ROOT_INDEX;
    while (LEFT_INDEX(i) < len || RIGHT_INDEX(i) < len) {
        size_t left = LEFT_INDEX(i);
        size_t right = RIGHT_INDEX(i);

        size_t min_index;
        if (left < len && right < len) {
            int cmp = queue->cmp_function(*ARRAYLIST_GET(queue->array, left, void *),
                                          *ARRAYLIST_GET(queue->array, right, void *));

            min_index = cmp <= 0 ? left : right;
        } else {

            assert(left < len && !(right < len));

            min_index = left;
        }

        void* min_value = *ARRAYLIST_GET(queue->array, min_index, void*);
        if (queue->cmp_function(last, min_value) <= 0) {
            *ARRAYLIST_GET(queue->array, i, void*) = last;
            break;
        } else {
            *ARRAYLIST_GET(queue->array, i, void*) = min_value;
            i = min_index;
        }
    }

    arraylist_remove(queue->array, len - 1, len);

    return min;
}

void* priority_queue_get_min(priority_queue_t* queue)
{
    assert(priority_queue_len(queue) > 0);

    return *ARRAYLIST_GET(queue->array, ROOT_INDEX, void *);
}

void priority_queue_destory(priority_queue_t* queue)
{
    arraylist_destory(queue->array);
    fds_free(queue);
}

size_t priority_queue_len(priority_queue_t* queue)
{
    return arraylist_len(queue->array);
}
