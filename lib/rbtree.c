#include <string.h>
#include <assert.h>
#include "alloc.h"
#include "rbtree.h"

#define COLOR_BLACK 0
#define COLOR_RED 1

static rbtree_node_t* find(rbtree_node_t* node, void* key, CmpFunc cmp_function);
static void insert(rbtree_node_t** p_root, void* key, void* value, CmpFunc cmp_function);
static rbtree_node_t* create_node(void* key, void* value, char color);
static void rotate(rbtree_node_t** p_node);
inline static char node_color(rbtree_node_t* node);
inline static rbtree_node_t** next(rbtree_node_t* node, void* key, CmpFunc cmp_function);

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
    insert(&(tree->root), key, value, tree->cmp_function);
    tree->root->color = COLOR_BLACK;
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

static rbtree_node_t* find(rbtree_node_t* node, void* key, CmpFunc cmp_function)
{
    if (node == NULL) {
        return NULL;
    } else {
        int cmp = cmp_function(key, node->key);
        if (cmp < 0) {
            return find(node->left, key, cmp_function);
        } else if (cmp > 0) {
            return find(node->right, key, cmp_function);
        } else {
            return node;
        }
    }
}

static void insert(rbtree_node_t** p_root, void* key, void* value, CmpFunc cmp_function)
{
    rbtree_node_t** p_grandparent = NULL;
    rbtree_node_t** p_parent = NULL;
    rbtree_node_t** p_node = p_root;

    while (*p_node != NULL) {
        if (node_color((*p_node)->left) == COLOR_RED && node_color((*p_node)->right) == COLOR_RED) {

            assert((*p_node)->left != NULL && (*p_node)->right != NULL);

            (*p_node)->color = COLOR_RED;
            (*p_node)->left->color = COLOR_BLACK;
            (*p_node)->right->color = COLOR_BLACK;

            if (p_grandparent != NULL) {
                rotate(p_grandparent);
            } else {
                assert(p_parent == NULL || p_parent == p_root);
            }
        }

        p_grandparent = p_parent;
        p_parent = p_node;
        p_node = next(*p_node, key, cmp_function);
        if (p_node == NULL) {
            return;
        }
    }

    *p_node = create_node(key, value, COLOR_RED);
    if (p_grandparent != NULL) {
        rotate(p_grandparent);
    } else {
        assert(p_parent == NULL || p_parent == p_root);
    }
}

inline rbtree_node_t** next(rbtree_node_t* node, void* key, CmpFunc cmp_function)
{
    assert(node != NULL);

    int cmp = cmp_function(key, node->key);
    if (cmp < 0) {
        return &node->left;
    } else if (cmp > 0) {
        return &node->right;
    } else {
        return NULL;
    }
}

static int check_struct(rbtree_node_t* node); /* 返回路径上黑色节点的数目。如果不满足红黑树路径节点数量的要求或红色节点的要求，返回负数 */
bool rbtree_check_struct(rbtree_t* tree)
{
    return node_color(tree->root) == COLOR_BLACK && check_struct(tree->root) > 0;
}

static int check_struct(rbtree_node_t* node)
{
    if (node == NULL) {
        return 0;
    }

    int left_black_count = check_struct(node->left);
    int right_black_count = check_struct(node->right);
    if ((left_black_count < 0 || right_black_count < 0 || left_black_count != right_black_count) ||
        (node_color(node) == COLOR_RED &&
         (node_color(node->left) == COLOR_RED || node_color(node->right) == COLOR_RED))) {
        return -1;
    } else {
        return left_black_count + (node_color(node) == COLOR_BLACK ? 1 : 0);
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

static void single_rotate_with_left(rbtree_node_t** p_root);
static void single_rotate_with_right(rbtree_node_t** p_root);
static void double_roate_with_left(rbtree_node_t** p_root);
static void double_roate_with_right(rbtree_node_t** p_root);

static void rotate(rbtree_node_t** p_node)
{
    if (node_color((*p_node)->left) == COLOR_RED) {
        rbtree_node_t* son = (*p_node)->left;
        if (node_color(son->left) == COLOR_RED) { // LL型单旋转
            single_rotate_with_left(p_node);
        } else if (node_color(son->right) == COLOR_RED) { // LR型双旋转
            double_roate_with_left(p_node);
        }
    } else if (node_color((*p_node)->right) == COLOR_RED) {
        rbtree_node_t* son = (*p_node)->right;
        if (node_color(son->left) == COLOR_RED) { // RL型双旋转
            double_roate_with_right(p_node);
        } else if (node_color(son->right) == COLOR_RED) { // RR型单旋转
            single_rotate_with_right(p_node);
        }
    }
}

static void single_rotate_with_left(rbtree_node_t** p_root) {
    rbtree_node_t* old_root = *p_root;
    rbtree_node_t* new_root = old_root->left;
    old_root->left = new_root->right;
    new_root->right = old_root;
    *p_root = new_root;

    new_root->color = COLOR_BLACK;
    old_root->color = COLOR_RED;
}


/* 旋转操作，要仔细推敲NULL指针情况，和旋转后的颜色 */
static void single_rotate_with_right(rbtree_node_t** p_root) {
    rbtree_node_t* old_root = *p_root;
    rbtree_node_t* new_root = old_root->right;
    old_root->right = new_root->left;
    new_root->left = old_root;
    *p_root = new_root;

    new_root->color = COLOR_BLACK;
    old_root->color = COLOR_RED;
}

static void double_roate_with_left(rbtree_node_t** p_root) {
    single_rotate_with_right(&(*p_root)->left);
    single_rotate_with_left(p_root);
}

static void double_roate_with_right(rbtree_node_t** p_root) {
    single_rotate_with_left(&(*p_root)->right);
    single_rotate_with_right(p_root);
}

inline static char node_color(rbtree_node_t* node)
{
    assert(node->color == COLOR_BLACK || node->color == COLOR_RED);

    return node == NULL ? COLOR_BLACK : node->color;
}
