#include "skiplist.h"

#include "alloc.h"
#include <assert.h>

/* 为方便编码，采用无穷大的尾节点 */
inline static bool is_end_node(skiplist_node_t* node)
{

    assert(node != NULL);

    return node->right == NULL;
}

inline static int cmp(void* key, skiplist_node_t* node, CmpFunc cmpf)
{
    return is_end_node(node) ? -1 : cmpf(key, node->key);
}

static skiplist_node_t* find(skiplist_node_t* first, void* key, CmpFunc cmpf);
static void insert_before(skiplist_node_t* prev, void* key, void* value);
static skiplist_node_t* create_end_node(skiplist_node_t* right, skiplist_node_t* down);
static void remove_skiplist_node(skiplist_node_t* prev, skiplist_node_t* node, bool is_indexed);
static skiplist_node_t* segment_last(skiplist_node_t* up_node);

skiplist_t* skiplist_create(CmpFunc cmp_function)
{
    skiplist_t* list = FDS_NEW(skiplist_t, 1);
    list->head = create_end_node(NULL, NULL);
    list->cmp_function = cmp_function;
    return list;
}

bool skiplist_exists(skiplist_t* list, void* key)
{
    return find(list->head, key, list->cmp_function) != NULL;
}

void* skiplist_get(skiplist_t* list, void* key, void* default_value)
{
    skiplist_node_t* node = find(list->head, key, list->cmp_function);
    return node == NULL ? default_value : node->value;
}

static skiplist_node_t* find(skiplist_node_t* node, void* key, CmpFunc cmpf)
{
    assert(is_end_node(node));

    while (node != NULL) {

        int c = cmp(key, node, cmpf);
        if (c > 0) {
            node = node->right;
        } else if (c < 0) {
            node = node->down;
        } else {
            return node;
        }
    }

    return NULL;
}

void skiplist_put(skiplist_t* list, void* key, void* value)
{
    skiplist_node_t* level_node = list->head;
    while (level_node != NULL) {

        skiplist_node_t* node = level_node;
        while (cmp(key, node, list->cmp_function) > 0) {
            node = node->right;
        }

        assert(node != NULL);

        if (node->down != NULL) {

            bool need_split;
            if (is_end_node(node)) {
                skiplist_node_t* drr = node->down->right->right;
                need_split = drr != NULL && !is_end_node(drr);
            } else {
                need_split = cmp(node->key, node->down->right->right, list->cmp_function) < 0;
            }

            if (need_split) {
                /* 下层对应的段有3个节点，因此将中间那个节点提升到本层来 */
                insert_before(node, node->down->right->key, node->down->right->value);
                node->right->down = node->down->right->right;
            }

        } else {
            insert_before(node, key, value);
            node->right->down = NULL;
        }

        level_node = node->down;
    }

    if (!is_end_node(list->head)) {
        list->head = create_end_node(NULL, list->head);
    }
}

/* 将数据插入到给定节点之前的辅助函数。注意实现方式。 */
static void insert_before(skiplist_node_t* prev, void* key, void* value)
{
    skiplist_node_t* new_node = FDS_NEW(skiplist_node_t, 1);
    new_node->right = prev->right;
    prev->right = new_node;

    new_node->key = prev->key;
    new_node->value = prev->value;

    prev->key = key;
    prev->value = value;
}

void skiplist_remove(skiplist_t* list, void* key)
{
    skiplist_node_t* level_node = list->head;
    while (level_node != NULL) {

        skiplist_node_t* node = level_node;
        skiplist_node_t* prev = NULL;
        while (cmp(key, node, list->cmp_function) > 0) {
            prev = node;
            node = node->right;
        }

#define SEG_EQ_1(n) (is_end_node(n) ? is_end_node(node->down->right) : cmp((n)->key, (n)->down->right, list->cmp_function) == 0)

        if (node->down == NULL) {

            remove_skiplist_node(prev, node, node == level_node);

        } else if (cmp(key, node, list->cmp_function) == 0) {

            assert(!is_end_node(node));

            if (SEG_EQ_1(node)) {
                remove_skiplist_node(prev, node, node == level_node);
            } else {
                skiplist_node_t* seg_last_node = segment_last(node);
                node->key = seg_last_node->key;
                node->value = seg_last_node->value;
            }

        } else if (SEG_EQ_1(node)) {

            if (node == level_node) { /* 向右合并 */
                if (SEG_EQ_1(node->right)) {
                    remove_skiplist_node(prev, node, node == level_node);
                } else {
                    node->key = node->right->down->key;
                    node->value = node->right->down->value;
                    node->right->down = node->right->down->right;
                }
            } else { /* 向左合并 */

                assert(prev->right == node);

                if (SEG_EQ_1(prev)) {
                    *prev = *node;
                    fds_free(node);
                } else {
                    skiplist_node_t* last = segment_last(prev);
                    node->down = last;
                    prev->key = last->key;
                    prev->value = last->value;
                }
            }
        }

            level_node = node->down;
    }
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

static skiplist_node_t* segment_last(skiplist_node_t* up_node)
{
    assert(!is_end_node(up_node));
    assert(up_node->down != NULL);

    skiplist_node_t* last;
    for (skiplist_node_t* it = up_node->down; it->right != up_node->right->down; it = it->right) {
        last = it;
    }
    return last;
}

static skiplist_node_t* create_end_node(skiplist_node_t* right, skiplist_node_t* down)
{
    skiplist_node_t* node = FDS_NEW(skiplist_node_t, 1);
    node->key = 0x0;
    node->value = 0x0;
    node->right = right;
    node->down = down;
    return node;
}

static void remove_skiplist_node(skiplist_node_t* prev, skiplist_node_t* node, bool is_indexed)
{
    skiplist_node_t* del_node;
    if (is_indexed) {
        del_node = node->right;
        *node = *del_node;
    } else {

        assert(prev->right == node);

        del_node = node;
        prev->right = node->right;
    }
    fds_free(del_node);
}
