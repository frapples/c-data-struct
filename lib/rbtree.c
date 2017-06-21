#include <string.h>
#include <assert.h>
#include "alloc.h"
#include "rbtree.h"

#define COLOR_BLACK 0
#define COLOR_RED 1

static rbtree_node_t* find(rbtree_node_t* node, void* key, CmpFunc cmp_function);
static void insert(rbtree_node_t** p_parent, rbtree_node_t** p_node, void* key, void* value, CmpFunc cmp_function);
static rbtree_node_t* create_node(void* key, void* value, char color);
static void rotate(rbtree_node_t** p_node);
inline static char node_color(rbtree_node_t* node);

rbtree_t* rbtree_create(CmpFunc cmp_function)
{
    rbtree_t* tree = FDS_NEW(rbtree_t, 1);
    tree->root = NULL;
    tree->cmp_function = cmp_function;
    return tree;
}

rbtree_t* rbtree_create_strkey()
{
    return rbtree_create((CmpFunc)strcmp);
}

void rbtree_put(rbtree_t* tree, void* key, void* value)
{
    if (tree->root == NULL) {
        tree->root = create_node(key, value, COLOR_BLACK);
    } else {
        int cmp = tree->cmp_function(key, tree->root);
        if (cmp < 0) {
            insert(&(tree->root), &tree->root->left, key, value, tree->cmp_function);
        } else if (cmp > 0) {
            insert(&(tree->root), &tree->root->right, key, value, tree->cmp_function);
        } else {
            // 不插入已经存在的key
        }
        tree->root->color = COLOR_BLACK;
    }
}

bool rbtree_exists(rbtree_t* tree, void* key)
{
    return find(tree->root, key, tree->cmp_function) != NULL;
}

void* rbtree_get(rbtree_t* tree, void* key, void* default_value)
{
    rbtree_node_t* node = find(tree->root, key, tree->cmp_function);
    return node != NULL ? node->value : default_value;
}

void rbtree_remove(rbtree_t* tree, void* key)
{
    assert(false && "Not implementation");
}

void rbtree_destory(rbtree_t* tree)
{
    if (tree->root == NULL) {
        fds_free(tree);
    } else {
        rbtree_node_t* node = tree->root;

        tree->root = node->left;
        rbtree_destory(tree);

        tree->root = node->right;
        rbtree_destory(tree);

        fds_free(tree->root);
    }
}

static void insert(rbtree_node_t** p_parent, rbtree_node_t** p_node, void* key, void* value, CmpFunc cmp_function)
{
    assert(*p_parent != NULL);
    assert((*p_parent)->left == *p_node || (*p_parent)->right == *p_node);

    if (*p_node == NULL) {
        *p_node = create_node(key, value, COLOR_RED);
        rotate(p_parent);
        return;
    }

    if (node_color((*p_node)->left) == COLOR_RED && node_color((*p_node)->right) == COLOR_RED) {

        assert((*p_node)->left != NULL && (*p_node)->right != NULL);

        (*p_node)->color = COLOR_RED;
        (*p_node)->left->color = COLOR_BLACK;
        (*p_node)->right->color = COLOR_BLACK;

        rotate(p_parent);
    }

    int cmp = cmp_function(key, (*p_node)->key);
    if (cmp < 0) {
        insert(p_node, &(*p_node)->left, key, value, cmp_function);
    } else if (cmp > 0) {
        insert(p_node, &(*p_node)->right, key, value, cmp_function);
    } else {
        return;
    }
}

static rbtree_node_t* create_node(void* key, void* value, char color)
{
    rbtree_node_t* node = FDS_NEW(rbtree_node_t, 1);
    node->left = NULL;
    node->right = NULL;
    node->key = key;
    node->value = value;
    node->color = color;
    return node;
}

static void rotate(rbtree_node_t** p_node)
{
    if (node_color(*p_node)) {
        assert(false && "Not implementation");
    }
}

inline static char node_color(rbtree_node_t* node)
{
    return node == NULL ? COLOR_BLACK : node->color;
}
