#include "avltree.h"

#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include "alloc.h"

static void insert(avltree_node_t** root, avltree_node_t* key, avltree_node_t* value, CmpFunc cmp_function);
static void remove(avltree_node_t** root, avltree_node_t* key, CmpFunc cmp_function);
static avltree_node_t* find(avltree_node_t* root, avltree_node_t* key, CmpFunc cmp_function);
static avltree_node_t* min_node(avltree_node_t* node);
static void destory(avltree_node_t* root);

static void rotate(avltree_node_t** p_root, bool debug__is_remove);
static void single_rotate_with_left(avltree_node_t** node);
static void single_rotate_with_right(avltree_node_t** node);
static void double_rotate_with_left(avltree_node_t** node);
static void double_rotate_with_right(avltree_node_t** node);

/* 非常好的考虑NULL的情况 */
static inline int height(avltree_node_t* node);
static inline void update_height(avltree_node_t* node);
static inline int max(int a, int b);

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
    return find(tree->root, key, tree->cmp_function) != NULL;
}

void* avltree_get(avltree_t* tree, void* key, void* default_value)
{
    avltree_node_t* node = find(tree->root, key, tree->cmp_function);
    return node == NULL ? default_value : node->value;
}

void avltree_remove(avltree_t* tree, void* key)
{
    remove(&tree->root, key, tree->cmp_function);
}

void avltree_destory(avltree_t* tree)
{
    destory(tree->root);
    fds_free(tree);
}

static void insert(avltree_node_t** p_root, avltree_node_t* key, avltree_node_t* value, CmpFunc cmp_function)
{
    if (*p_root == NULL) {
        avltree_node_t* root = FDS_NEW(avltree_node_t, 1);
        root->left = NULL;
        root->right = NULL;
        root->height = 0;
        root->key = key;
        root->value = value;
        *p_root = root;

#ifndef NDEBUG
        root->debug_height = root->height;
#endif // NDEBUG

        return;
    }

    avltree_node_t* root = *p_root;

    assert(root->height == root->debug_height);

    int cmp = cmp_function(key, root->key);
    if (cmp < 0) {
        insert(&root->left, key, value, cmp_function);
    } else if (cmp > 0) {
        insert(&root->right, key, value, cmp_function);
    } else {
        root->value = value;
    }

    update_height(root);

    if (abs(height(root->left) - height(root->right)) > 1) {
        rotate(p_root, false);
    }
}

static void remove(avltree_node_t** p_node, avltree_node_t* key, CmpFunc cmp_function)
{
    if (*p_node == NULL) {
        return;
    }

    int cmp = cmp_function(key, (*p_node)->key);
    if (cmp < 0) {
        remove(&(*p_node)->left, key, cmp_function);
    } else if (cmp > 0) {
        remove(&(*p_node)->right, key, cmp_function);
    } else if ((*p_node)->left == NULL) {
            avltree_node_t* node = *p_node;
            *p_node = (*p_node)->right;
            fds_free(node);
            return;
    } else if ((*p_node)->right == NULL) {
            avltree_node_t* node = *p_node;
            *p_node = (*p_node)->left;
            fds_free(node);
            return;
    } else {
        avltree_node_t* min = min_node((*p_node)->right);
        (*p_node)->key = min->key;
        (*p_node)->value = min->key;
        remove(&(*p_node)->right, min->key, cmp_function);
    }

    update_height(*p_node);
    if (abs(height((*p_node)->left) - height((*p_node)->right)) > 1) {
        rotate(p_node, true);
    }
}

static avltree_node_t* find(avltree_node_t* root, avltree_node_t* key, CmpFunc cmp_function)
{
    if (root == NULL) {
        return root;
    }

    assert(root->height == root->debug_height);

    int cmp = cmp_function(key, root->key);
    if (cmp < 0) {
        return find(root->left, key, cmp_function);
    } else if (cmp > 0) {
        return find(root->right, key, cmp_function);
    } else {
        return root;
    }
}

static void destory(avltree_node_t* root)
{
    if (root != NULL) {

        assert(root->height == root->debug_height);

        destory(root->left);
        destory(root->right);
        fds_free(root);
    }
}


static void rotate(avltree_node_t** p_root, bool debug__is_remove)
{
    avltree_node_t* root = *p_root;
    int diff = height(root->left) - height(root->right);

    assert(abs(diff <= 2));

    /* 要注意退化的avl树 */
    /* 有趣的是根据高度来判断旋转情况，对移除节点也有效 */
    if (diff == 2) {
        int son_diff = height(root->left->left) - height(root->left->right);

        assert(abs(son_diff) <= 1);

        if (son_diff > 0) {
            single_rotate_with_left(p_root); // 左-左
        } else if (son_diff < 0) {
            double_rotate_with_left(p_root); // 左-右
        } else {
            /* 删除节点带来的旋转，可能会出现这种情况 */

            assert(debug__is_remove);

            single_rotate_with_left(p_root);
        }
    } else if (diff == -2) {
        int son_diff = height(root->right->left) - height(root->right->right);

        assert(abs(son_diff) <= 1);

        if (son_diff > 0) {
            double_rotate_with_right(p_root); // 右-左
        } else if (son_diff < 0) {
            single_rotate_with_right(p_root); // 右-右
        } else {

            assert(debug__is_remove);

            single_rotate_with_right(p_root);
        }
    } else {
        assert(false);
    }
}

/* 要注意能否处理退化的avl树 */
static void single_rotate_with_left(avltree_node_t** node)
{
    avltree_node_t* old_root = *node;
    avltree_node_t* new_root = (*node)->left;
    (*node)->left = new_root->right;
    new_root->right = *node;
    *node = new_root;

    /* 重要：顺序不能反 */
    update_height(old_root);
    update_height(new_root);
}

static void single_rotate_with_right(avltree_node_t** node)
{
    avltree_node_t* old_root = *node;
    avltree_node_t* new_root = (*node)->right;
    (*node)->right = new_root->left;
    new_root->left = *node;
    *node = new_root;

    /* 重要：顺序不能反 */
    update_height(old_root);
    update_height(new_root);
}

static void double_rotate_with_left(avltree_node_t** node)
{
    single_rotate_with_right(&(*node)->left);
    single_rotate_with_left(node);
}

static void double_rotate_with_right(avltree_node_t** node)
{
    single_rotate_with_left(&(*node)->right);
    single_rotate_with_right(node);
}

static avltree_node_t* min_node(avltree_node_t* node)
{
    assert(node != NULL);

    return node->left == NULL ? node : min_node(node->left);
}

static inline int max(int a, int b)
{
    return a > b ? a : b;
}

static inline int height(avltree_node_t* node)
{
    return node == NULL ? -1 : node->height;
}

static inline void update_height(avltree_node_t* node)
{
    node->height = max(height(node->left), height(node->right)) + 1;

#ifndef NDEBUG
    node->debug_height = node->height;
#endif // NDEBUG
}
