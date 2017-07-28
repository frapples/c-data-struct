#include "skiplist.h"

#include "alloc.h"

skiplist_t* skiplist_create(CmpFunc cmp_function)
{
    skiplist_t* list = FDS_NEW(skiplist_t, 1);
    list->head = FDS_NEW(skiplist_node_t, 1);
    list->head->key = 0x0;
    list->head->value = 0x0;
    list->head->right = NULL;
    list->head->down = NULL;
    list->cmp_function = cmp_function;
    return list;
}

bool skiplist_exists(skiplist_t* list, void* key)
{
    return false;
}

void* skiplist_get(skiplist_t* list, void* key, void* default_value)
{
    return NULL;
}

void skiplist_remove(skiplist_t* list, void* key)
{
}

static void destory_list(skiplist_node_t* list);
void skiplist_destory(skiplist_t* list)
{
    skiplist_node_t* node = list->head;
    while (node != NULL) {
        skiplist_node_t* next = node->down;
        destory_list(node);
        node = next;
    }
}

static void destory_list(skiplist_node_t* node)
{
    while (node != NULL) {
        skiplist_node_t* next = node->right;
        fds_free(node);
        node = next;
    }
}
