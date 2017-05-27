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

typedef struct{
    void* next;
}inline_node_t;

#define DECLARE_INLINE_LIST    inline_node_t __inline_node
#define INLINE_LIST_OFFSET(type) offsetof(type, __inline_node)
#define INLINE_LIST_NEXT(p, type) ((type *)(((inline_node_t*)((p) + INLINE_LIST_OFFSET(type)))->next))

void inline_list_preappend(void** node, void* insert_node, size_t struct_offset);
void inline_list_append(void** prenode, void* insert_node, size_t struct_offset);

void inline_list_remove(void** node,  size_t struct_offset);

bool inline_list_is_in(void* list, void* node, size_t struct_offset);


#endif /* __INLINELINKEDLIST_H__RAND68698913203068463 */
