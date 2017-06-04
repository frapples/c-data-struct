#include "avltree.h"

#include <string.h>
#include <assert.h>
#include "alloc.h"

static void insert(avltree_node_t** root, avltree_node_t* key, avltree_node_t* value, CmpFunc cmp_function);
static avltree_node_t** find(avltree_node_t** root, avltree_node_t* key, CmpFunc cmp_function);
static void destory(avltree_node_t* root);

avltree_t* avltree_create(CmpFunc cmp_function)
{
    avltree_t* tree = FDS_NEW(avltree_t, 1);
    tree->root = NULL;
    tree->cmp_function = cmp_function;
    return tree;
}

avltree_t* avltree_create_strkey()
{
    return avltree_create((CmpFunc)strcmp);
}

void avltree_put(avltree_t* tree, void* key, void* value)
{
    insert(&(tree->root), key, value, tree->cmp_function);
}

bool avltree_exists(avltree_t* tree, void* key)
{
    return *(find(&(tree->root), key, tree->cmp_function)) != NULL;
}

void* avltree_get(avltree_t* tree, void* key, void* default_value)
{
    avltree_node_t** p_node = find(&(tree->root), key, tree->cmp_function);
    return *p_node == NULL ? default_value : (*p_node)->value;
}

void avltree_remove(avltree_t* tree, void* key)
{
    /* TODO */
    assert(false);
}

void avltree_destory(avltree_t* tree)
{
    destory(tree->root);
    fds_free(tree);
}
