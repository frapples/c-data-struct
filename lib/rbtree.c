#include <string.h>
#include <assert.h>
#include "alloc.h"
#include "rbtree.h"

#define COLOR_BLACK 0
#define COLOR_RED 1

static rbtree_node_t** find(rbtree_node_t** p_node, void* key, CmpFunc cmp_function);
static void insert(rbtree_node_t** p_root, void* key, void* value, CmpFunc cmp_function);
enum {
    REMOVE_MAX,
    REMOVE_MIN,
    REMOVE_KEY
};
static rbtree_node_t** remove_(rbtree_node_t** node, bool* p_is_left);
static void remove_black_leaf(rbtree_node_t** p_node, bool is_left, void* key, CmpFunc cmp_function);
static rbtree_node_t* create_node(void* key, void* value, char color);
static void rotate_with_double_red(rbtree_node_t** p_node);
static void rotate_with_make_son_red(rbtree_node_t** p_node);
static rbtree_node_t** rotate_with_make_parent_red(rbtree_node_t** p_node);
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
    return find(&tree->root, key, tree->cmp_function) != NULL;
}

void* rbtree_get(rbtree_t* tree, void* key, void* default_value)
{
    rbtree_node_t** p_node = find(&tree->root, key, tree->cmp_function);
    return p_node != NULL ? (*p_node)->value : default_value;
}


void rbtree_remove(rbtree_t* tree, void* key)
{
    rbtree_node_t** p_delnode = find(&tree->root, key, tree->cmp_function);
    if (p_delnode == NULL) {
        return;
    }

    bool is_left;
    p_delnode = remove_(p_delnode, &is_left);
    /* 此时， 树中可能有两个节点拥有相同的key，这时：
       一个是非叶子节点A。
       一个是叶子节点B，是要最后删除的。
       当要寻找叶子节点B时，is_left变量指示当搜索到达节点A时应往那边走
     */

    assert(*p_delnode != NULL && (*p_delnode)->left == NULL && (*p_delnode)->right == NULL);

    rbtree_node_t* node = *p_delnode;
    if (node_color(*p_delnode) == COLOR_BLACK) {
        remove_black_leaf(&tree->root, is_left, (*p_delnode)->key, tree->cmp_function);
        tree->root->color = COLOR_RED;
    }

    *p_delnode = NULL;
    fds_free(node);
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

static rbtree_node_t** find(rbtree_node_t** p_node, void* key, CmpFunc cmp_function)
{
    if (*p_node == NULL) {
        return NULL;
    } else {
        int cmp = cmp_function(key, (*p_node)->key);
        if (cmp < 0) {
            return find(&(*p_node)->left, key, cmp_function);
        } else if (cmp > 0) {
            return find(&(*p_node)->right, key, cmp_function);
        } else {
            return p_node;
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

static rbtree_node_t** remove_(rbtree_node_t** p_node, bool* p_is_left)
{
    assert(*p_node != NULL);

    rbtree_node_t** p_replace;
    if ((*p_node)->left != NULL) {
        // 找左子树最大值
        *p_is_left = true;
        p_replace =&(*p_node)->left;
        while ((*p_replace)->right != NULL) {
            p_replace = &(*p_replace)->right;
        }

    } else if ((*p_node)->right != NULL) {
        // 找右子树最小值
        *p_is_left = false;
        p_replace = &(*p_node)->right;
        while ((*p_replace)->left != NULL) {
            p_replace = &(*p_replace)->left;
        }
    } else {
        return p_node;
    }

    (*p_node)->key = (*p_replace)->key;
    (*p_node)->value = (*p_replace)->value;
    return remove_(p_replace, p_is_left);
}

static rbtree_node_t** remove_black_leaf__next(rbtree_node_t* node, bool is_left, void* key, CmpFunc cmp_function);
static void remove_black_leaf(rbtree_node_t** p_root, bool is_left, void* key, CmpFunc cmp_function)
{
    rbtree_node_t** p_parent = NULL;
    rbtree_node_t** p_node = p_root;

    rbtree_node_t** p_next = remove_black_leaf__next(*p_node, is_left, key, cmp_function);
    while (p_node != NULL) {
        bool son_both_black = node_color((*p_node)->left) == COLOR_BLACK && node_color((*p_node)->right) == COLOR_BLACK;
        if (p_parent == NULL && son_both_black) {

            (*p_node)->color = COLOR_RED;

            p_parent = p_node, p_node = p_next;
        } else if (p_parent != NULL && son_both_black) {

            assert(node_color(*p_parent) == COLOR_RED);

            rbtree_node_t* brother = &(*p_parent)->left == p_node ? (*p_parent)->right : (*p_parent)->left;

            assert(node_color(*p_node) == COLOR_BLACK);
            assert(brother != NULL);

            rbtree_node_t* node = *p_node;
            if (node_color(brother->left) == COLOR_BLACK && node_color(brother->right) == COLOR_BLACK) {
                /* 颜色翻转*/
                (*p_parent)->color = COLOR_BLACK;
                (*p_node)->color = COLOR_RED;
                brother->color = COLOR_RED;
            } else {

                rotate_with_make_son_red(p_parent);

                assert(node == *p_node);
                assert(node_color(*p_node) == COLOR_RED);

            }

            assert((*p_node)->color == COLOR_RED);

            p_parent = p_node, p_node = p_next;
        } else /* 无论p_parent 是不是 NULL，!son_both_black时 */ {
            /* 往下走一层 */
            p_parent = p_node, p_node = p_next;

            if (p_node != NULL) {
                p_next = remove_black_leaf__next(*p_node, is_left, key, cmp_function);

                if (node_color(*p_node) == COLOR_BLACK) {

                    p_parent = rotate_with_make_parent_red(p_parent);

                    assert(node_color(*p_parent) == COLOR_RED && node_color(*p_node) == COLOR_BLACK);

                } else /* (node_color(*p_node) == COLOR_RED) */ {
                    p_parent = p_node, p_node = p_next;
                }
            } else {

                assert(false);
            }
        }

        assert(node_color(*p_parent) == COLOR_RED);

        if (p_node != NULL) {
            p_next = remove_black_leaf__next(*p_node, is_left, key, cmp_function);
        } else {
            p_next = NULL;
        }
    }
}

static rbtree_node_t** remove_black_leaf__next(rbtree_node_t* node, bool is_left, void* key, CmpFunc cmp_function)
{
    int cmp = cmp_function(key, node->key);

    rbtree_node_t** p_next;
    if (cmp < 0) {
        p_next = &node->left;
    } else if (cmp > 0) {
        p_next = &node->right;
    } else {
        if (node->left == NULL && node->right == NULL) {
            return NULL;
        } else {
            p_next = is_left ? &node->left : &node->right;
        }
    }

    assert(*p_next != NULL);

    return p_next;
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
   旋转后：parent仍然是node父亲，parent红，node仍然黑
   返回值：（重要），为了方便旋转，传入的是二级指针。旋转后该二级指针指向非原本项而是旋转后的根节点。
   返回原本项的二级指针
*/
static rbtree_node_t** rotate_with_make_parent_red(rbtree_node_t** p_node)
{
    assert(*p_node != NULL);

    bool left_red = node_color((*p_node)->left) == COLOR_RED;
    bool right_red = node_color((*p_node)->right) == COLOR_RED;
    if (left_red && !right_red) {
        single_rotate_with_left(p_node);
        (*p_node)->right->color = COLOR_RED;
        return &(*p_node)->right;
    } else if (!left_red && right_red) {
        single_rotate_with_right(p_node);
        (*p_node)->left->color = COLOR_RED;
        return &(*p_node)->left;
    } else {
        return NULL;
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
