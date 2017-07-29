#include "skiplist.h"

#include "alloc.h"
#include <assert.h>

/* 为方便编码，采用无穷大的尾节点 */
inline static bool is_end_node(skiplist_node_t* node)
{
    return node->right == NULL;
}

inline static int cmp(void* key, skiplist_node_t* node, CmpFunc cmpf)
{
    return is_end_node(node) ? -1 : cmpf(key, node->key);
}

static skiplist_node_t* find(skiplist_node_t* first, void* key, CmpFunc cmpf);

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
    return !is_end_node(find(list->head, key, list->cmp_function));
}

void* skiplist_get(skiplist_t* list, void* key, void* default_value)
{
    skiplist_node_t* node = find(list->head, key, list->cmp_function);

    assert(is_end_node(node) || list->cmp_function(key, node->key));

    return is_end_node(node) ? default_value : node->value;
}

static skiplist_node_t* find(skiplist_node_t* first, void* key, CmpFunc cmpf)
{
    assert(is_end_node(first));

    skiplist_node_t* node = first;
    skiplist_node_t* next = first;
    while (next != NULL) {
        node = next;

        next = cmp(key, node, cmpf) > 0 ? node->right : node->down;
    }

    return node;
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
