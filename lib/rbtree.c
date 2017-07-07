#include <string.h>
#include <assert.h>
#include "alloc.h"
#include "rbtree.h"

#define COLOR_BLACK 0
#define COLOR_RED 1

static rbtree_node_t* find(rbtree_node_t* node, void* key, CmpFunc cmp_function);
static void insert(rbtree_node_t** p_root, void* key, void* value, CmpFunc cmp_function);
enum {
    REMOVE_MAX,
    REMOVE_MIN,
    REMOVE_KEY
};
static rbtree_node_t* remove_(rbtree_node_t** p_parent, rbtree_node_t** p_node, int removed_type, void* key, CmpFunc cmp_function);
static rbtree_node_t* create_node(void* key, void* value, char color);
static void rotate_with_double_red(rbtree_node_t** p_node);
static void rotate_with_make_son_red(rbtree_node_t** p_node);
static void rotate_with_make_parent_red(rbtree_node_t** p_node);
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
    if (rbtree_exists(tree, key)) {
        /* 为方便删除代码实现，无需考虑不存在的情况 */
        rbtree_node_t* removed = remove_(NULL, &tree->root, REMOVE_KEY, key, tree->cmp_function);
            fds_free(removed);
        }
}

static void destory(rbtree_node_t* node);
void rbtree_destory(rbtree_t* tree)
{
    destory(tree->root);
    fds_free(tree);
}

static void destory(rbtree_node_t* node)
{
    if (node != NULL) {
        destory(node->left);
        destory(node->right);
        fds_free(node);
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
                if (node_color(*p_parent) == COLOR_RED && node_color(*p_node) == COLOR_RED) {
                    /*  旋转会改变树结构，因此，p_grandparent, p_parent, p_node指向的节点父子关系失效 */
                    /* 一种方法是，在旋转之后，把p_node 设为p_grandparent, 从*p_grandparent到p_node的循环再来一次 */
                    /* 显然， 重新从*p_grandparent循环到*p_node的过程中，不会发生新的旋转，循环也不会结束 */
                    rotate_with_double_red(p_grandparent);
                    p_node = p_grandparent;
                    p_grandparent = NULL;
                    p_parent = NULL;
                }
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
        rotate_with_double_red(p_grandparent);
    } else {
        assert(p_parent == NULL || p_parent == p_root);
    }
}

static inline bool remove__next(rbtree_node_t* node, int removed_type, void* key, CmpFunc cmp_function,
                                           rbtree_node_t*** pp_parent, rbtree_node_t*** pp_node);

static rbtree_node_t* remove_(rbtree_node_t** p_parent, rbtree_node_t** p_node, int removed_type, void* key, CmpFunc cmp_function)
{
    /* 这是一个递归函数。当p_parent为NULL时，p_node一定是整棵rbtreed 根节点 */
    assert(*p_node != NULL);

    bool finded = false;
    while (!finded) {
        bool son_both_black = node_color((*p_node)->left) == COLOR_BLACK && node_color((*p_node)->right) == COLOR_BLACK;
        if (p_parent == NULL && son_both_black) {

            (*p_node)->color = COLOR_RED;
            finded = !remove__next(*p_node, removed_type, key, cmp_function, &p_parent, &p_node);

        } else if (p_parent != NULL && son_both_black) {

            assert(node_color(*p_parent) == COLOR_RED);

            rbtree_node_t* brother = &(*p_parent)->left == p_node ? (*p_parent)->right : (*p_parent)->left;

            if (node_color(brother->left) == COLOR_BLACK && node_color(brother->right) == COLOR_BLACK) {
                /* 颜色翻转*/
                (*p_parent)->color = COLOR_BLACK;
                (*p_node)->color = COLOR_RED;
                brother->color = COLOR_RED;
            } else {
                rotate_with_make_son_red(p_parent);
            }

            finded = !remove__next(*p_node, removed_type, key, cmp_function, &p_parent, &p_node);

        } else /* 无论p_parent 是不是 NULL，!son_both_black时 */ {
            /* 往下走一层 */
            finded = !remove__next(*p_node, removed_type, key, cmp_function, &p_parent, &p_node);
            if (!finded) {
                if (node_color(*p_node) == COLOR_BLACK) {
                    rotate_with_make_parent_red(p_parent);
                } else /* (node_color(*p_node) == COLOR_RED) */ {
                    /* 什么都不用做*/
                }
            }
        }
    }


    /* 选择删除左子树的最大节点或右子树的最小节点，并用其代替待删除节点 */
    rbtree_node_t* replaced_node;
    if ((*p_node)->left != NULL) {
        replaced_node = remove_(p_node, &(*p_node)->left, REMOVE_MAX, NULL, NULL);

        assert(replaced_node != NULL);

    } else if ((*p_node)->right != NULL) {
        replaced_node = remove_(p_node, &(*p_node)->right, REMOVE_MIN, NULL, NULL);

        assert(replaced_node != NULL);

    } else {
        replaced_node = NULL;
    }

    rbtree_node_t* node = *p_node;
    *p_node = replaced_node;
    if (replaced_node != NULL) {
        replaced_node->left = node->left;
        replaced_node->right = node->right;
    }
    return node;
}

static inline bool remove__next(rbtree_node_t* node, int removed_type, void* key, CmpFunc cmp_function,
                                           rbtree_node_t*** pp_parent, rbtree_node_t*** pp_node)
{
    rbtree_node_t** p_next;
    if (removed_type == REMOVE_KEY) {
        p_next = next(node, key, cmp_function);

        assert(p_next == NULL || *p_next != NULL);

    } else if (removed_type == REMOVE_MIN) {
        p_next = &node->left;
    } else if (removed_type == REMOVE_MAX) {
        p_next = &node->right;
    } else {
        assert(false);
    }

    if (p_next == NULL || *p_next == NULL) {
        return false;
    } else {
        return p_next;
        *pp_parent = *pp_node;
        *pp_node = p_next;
        return true;
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

/* 此旋转用于插入过程中。 设grandparent, parent, uncle, self
   旋转前：parent和self为红色（破坏红黑树条件），uncle为黑色
   旋转后：上述各节点的父子关系被破坏，树结构被旋转以满足红黑树条件。
 */
static void rotate_with_double_red(rbtree_node_t** p_node)
{
    assert(*p_node != NULL);

    bool rotated = true;
    if (node_color((*p_node)->left) == COLOR_RED) {
        rbtree_node_t* son = (*p_node)->left;
        if (node_color(son->left) == COLOR_RED) { // LL型单旋转
            single_rotate_with_left(p_node);
        } else if (node_color(son->right) == COLOR_RED) { // LR型双旋转
            double_roate_with_left(p_node);
        } else {
            rotated = false;
        }
    } else if (node_color((*p_node)->right) == COLOR_RED) {
        rbtree_node_t* son = (*p_node)->right;
        if (node_color(son->left) == COLOR_RED) { // RL型双旋转
            double_roate_with_right(p_node);
        } else if (node_color(son->right) == COLOR_RED) { // RR型单旋转
            single_rotate_with_right(p_node);
        } else {
            rotated = false;
        }
    } else {
        rotated = false;
    }

    if (rotated) {
        (*p_node)->color = COLOR_BLACK;
        (*p_node)->left->color = COLOR_RED;
        (*p_node)->right->color = COLOR_RED;
    }
}

/* 此旋转用于删除过程中。设parent, self, brother
   旋转前：parent红色，self黑色，brother黑色，self儿子都是黑色，brother拥有红色儿子
   旋转后：parent和self的父子关系不变，self一定会变为红色
 */
static void rotate_with_make_son_red(rbtree_node_t** p_node)
{
    assert(*p_node != NULL);
    assert((*p_node)->left != NULL);
    assert((*p_node)->right != NULL);

    bool rotated = true;
    rbtree_node_t* node;
    if (node_color((*p_node)->left->left) == COLOR_RED) { // LL
        node = (*p_node)->right;
        single_rotate_with_left(p_node);
    }  else if (node_color((*p_node)->left->right) == COLOR_RED) { // LR
        node = (*p_node)->right;
        double_roate_with_left(p_node);
    } else if (node_color((*p_node)->right->left) == COLOR_RED) { // RL
        node = (*p_node)->left;
        double_roate_with_right(p_node);
    }  else if (node_color((*p_node)->right->right) == COLOR_RED) { // RR
        node = (*p_node)->left;
        single_rotate_with_right(p_node);
    } else {
        rotated = false;
    }

    if (rotated) {
        (*p_node)->color = COLOR_RED;
        (*p_node)->left->color = COLOR_BLACK;
        (*p_node)->right->color = COLOR_BLACK;
        node->color = COLOR_RED;
    }
}

/* 此旋转用于删除过程中。设parent, self, brother
   旋转前：parent黑，node黑，brother红。
   旋转后：parent仍然是node父亲，parent红，node仍然黑 */
static void rotate_with_make_parent_red(rbtree_node_t** p_node)
{
    assert(*p_node != NULL);

    bool left_red = node_color((*p_node)->left) == COLOR_RED;
    bool right_red = node_color((*p_node)->right) == COLOR_RED;
    if (left_red && !right_red) {
        single_rotate_with_left(p_node);
        (*p_node)->right->color = COLOR_RED;
    } else if (!left_red && right_red) {
        single_rotate_with_right(p_node);
        (*p_node)->left->color = COLOR_RED;
    }
}

/* 旋转操作，要仔细推敲NULL指针情况 */
static void single_rotate_with_left(rbtree_node_t** p_root) {
    rbtree_node_t* old_root = *p_root;
    rbtree_node_t* new_root = old_root->left;
    old_root->left = new_root->right;
    new_root->right = old_root;
    *p_root = new_root;
}

static void single_rotate_with_right(rbtree_node_t** p_root) {
    rbtree_node_t* old_root = *p_root;
    rbtree_node_t* new_root = old_root->right;
    old_root->right = new_root->left;
    new_root->left = old_root;
    *p_root = new_root;
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
    assert(node == NULL || node->color == COLOR_BLACK || node->color == COLOR_RED);

    return node == NULL ? COLOR_BLACK : node->color;
}
