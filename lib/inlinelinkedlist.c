#include "inlinelinkedlist.h"


void inline_list_preappend(inline_node_t** node, inline_node_t* insert_node)
{
    insert_node->next = *node;
    *node = insert_node;
}

void inline_list_append(inline_node_t** prenode, inline_node_t* insert_node)
{
    if (*prenode != NULL) {
        insert_node->next = (*prenode)->next;
        (*prenode)->next = insert_node;
    } else {
        *prenode = insert_node;
    }
}


void inline_list_remove(inline_node_t** node)
{
    *node = (*node)->next;
}


bool inline_list_is_in(inline_node_t* list, inline_node_t* node)
{
    for (inline_node_t* cur = list; cur != NULL; cur = cur->next) {
        if (cur == node) {
            return true;
        }
    }
    return false;
}
