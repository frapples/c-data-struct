#ifndef __INLINELINKEDLIST_H__RAND68698913203068463
#define __INLINELINKEDLIST_H__RAND68698913203068463

#include <stddef.h>
#include <stdbool.h>

/* 嵌入式的单向链表，不负责内存管理 */

/*
  几个要点：
  1. 链表是递归形的数据结构，任意一个节点都能当成一个完整的链表
  2. 使用节点指针的指针代表链表，当指针为NULL时为空链表
 */

typedef struct inline_node_s inline_node_t;
struct inline_node_s{
    struct inline_node_s* next;
};

#define DECLARE_INLINE_LIST    inline_node_t __inline_node
#define FROM_INLINE_LIST(type, p) ((type*)((p) - offsetof(type, __inline_node)))
#define TO_INLINE_LIST(p) (&((p)->__inline_node))
#define INLINE_LIST_NEXT(type, p) (FROM_INLINE_LIST(type, TO_INLINE_LIST(p)->next))

void inline_list_preappend(inline_node_t** node, inline_node_t* insert_node);
void inline_list_append(inline_node_t** prenode, inline_node_t* insert_node);

void inline_list_remove(inline_node_t** node);

bool inline_list_is_in(inline_node_t* list, inline_node_t* node);


#endif /* __INLINELINKEDLIST_H__RAND68698913203068463 */
