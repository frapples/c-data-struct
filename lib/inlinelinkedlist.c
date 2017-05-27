#include "inlinelinkedlist.h"


#define NEXT(p, offset) (((inline_node_t*)((p) + offset))->next)

void inline_list_preappend(void** node, void* insert_node, size_t struct_offset)
{
    NEXT(insert_node, struct_offset) = *node;
    *node = insert_node;
}

void inline_list_append(void** prenode, void* insert_node, size_t struct_offset)
{
    if (*prenode != NULL) {
        NEXT(insert_node, struct_offset) = NEXT(*prenode, struct_offset);
        NEXT(*prenode, struct_offset) = insert_node;
    } else {
        NEXT(insert_node, struct_offset) = NULL;
        *prenode = insert_node;
    }
}


void inline_list_remove(void** node,  size_t struct_offset)
{
    *node = NEXT(*node, struct_offset);
}


bool inline_list_is_in(void* list, void* node, size_t struct_offset)
{
    for (void* cur = list; cur != NULL; cur = NEXT(cur, struct_offset)) {
        if (cur == node) {
            return true;
        }
    }
    return false;
}

