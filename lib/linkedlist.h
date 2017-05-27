#ifndef __LINKEDLIST_H__RAND392633570394130931
#define __LINKEDLIST_H__RAND392633570394130931

#include "inlinelinkedlist.h"

typedef struct {
    DECLARE_INLINE_LIST;
    void* data;
}linked_node_t;




linked_node_t* linked_list_create_empty();
void linked_list_destory(linked_node_t* list);

void linked_list_append(linked_node_t** node, void* data);
void linked_list_preappend(linked_node_t** node, void* data);
void linked_list_remove(linked_node_t** node);

linked_node_t* linked_list_find(linked_node_t* list);

#endif /* __LINKEDLIST_H__RAND392633570394130931 */
