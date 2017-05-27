#include "linkedlist.h"
#include "inlinelinkedlist.h"
#include "alloc.h"


#define NEXT(p) LINKED_LIST_NEXT(p)
#define OFFSET INLINE_LIST_OFFSET(linked_node_t)

linked_node_t* linked_list_create_empty()
{
    return NULL;
}

inline static linked_node_t* create_node(void* data)
{
    linked_node_t* node = FDS_NEW(linked_node_t, 1);
    node->data = data;
    return node;
}

void linked_list_destory(linked_node_t** list)
{
    while (*list != NULL) {
        linked_node_t* next = NEXT(*list);
        fds_free(*list);
        *list = next;
    }
}


void linked_list_append(linked_node_t** node, void* data)
{
    inline_list_append((void**)node, create_node(data), OFFSET);
}

void linked_list_preappend(linked_node_t** node, void* data)
{
    inline_list_preappend((void**)node, create_node(data), OFFSET);
}

void linked_list_remove(linked_node_t** node)
{
    inline_list_remove((void**)node, OFFSET);
    fds_free(node);
}
