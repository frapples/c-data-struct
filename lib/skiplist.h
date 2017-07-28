#ifndef __SKIPLIST_H__RAND19849956753359072362213305895055947649
#define __SKIPLIST_H__RAND19849956753359072362213305895055947649

#include <stdbool.h>
#include "utils.h"

/* 1-2-3跳跃表的实现 */

typedef struct skiplist_node_s {
    struct skiplist_node_s* right;
    struct skiplist_node_s* down;
    void* key;
    void* value;
}skiplist_node_t;

typedef struct {
    skiplist_node_t* head;
    CmpFunc cmp_function;
}skiplist_t;

skiplist_t* skiplist_create(CmpFunc cmp_function);

void skiplist_put(skiplist_t* list, void* key, void* value);
bool skiplist_exists(skiplist_t* list, void* key);
void* skiplist_get(skiplist_t* list, void* key, void* default_value);
void skiplist_remove(skiplist_t* list, void* key);

void skiplist_destory(skiplist_t* list);

#endif /* __SKIPLIST_H__RAND19849956753359072362213305895055947649 */
