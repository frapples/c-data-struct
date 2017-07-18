#ifndef __PRIORITYQUEUE_H__RAND213696340737689456089384680985024673
#define __PRIORITYQUEUE_H__RAND213696340737689456089384680985024673

#include "arraylist.h"
#include "utils.h"

typedef struct {
    arraylist_t array;
    CmpFunc cmp_function;
}priority_queue_t;

priority_queue_t* priority_queue_create(CmpFunc);
priority_queue_t* priority_queue_insert(void* data);
void* priority_queue_delete_min(priority_queue_t* queue);
void* priority_queue_get_min(priority_queue_t* queue);
void priority_queue_destory(priority_queue_t* queue);
size_t priority_queue_len(priority_queue_t* queue);

#endif /* __PRIORITYQUEUE_H__RAND213696340737689456089384680985024673 */
