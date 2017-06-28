#ifndef __RBTREE_H__RAND4212532133787459912177547191307587405
#define __RBTREE_H__RAND4212532133787459912177547191307587405

#include <stdbool.h>
#include <stdio.h>
#include "utils.h"

typedef struct rbtree_node_s {
    struct rbtree_node_s* left;
    struct rbtree_node_s* right;
    void* key;
    void* value;
    char color;
}rbtree_node_t;

typedef struct {
    rbtree_node_t* root;
    CmpFunc cmp_function;
}rbtree_t;

rbtree_t* rbtree_create(CmpFunc cmp_function);
rbtree_t* rbtree_create_strkey();

void rbtree_put(rbtree_t* tree, void* key, void* value);
bool rbtree_exists(rbtree_t* tree, void* key);
void* rbtree_get(rbtree_t* tree, void* key, void* default_value);
void rbtree_remove(rbtree_t* tree, void* key);

void rbtree_destory(rbtree_t* tree);

/* 检查给定的rbtree是否满足红黑树的要求 */
bool rbtree_check_struct(rbtree_t* tree);

#endif /* __RBTREE_H__RAND4212532133787459912177547191307587405 */
