#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "google/cmockery.h"

#include "voidp.h"
#include "linkedlist.h"

void test_linked_list(void **state)
{
    int count = 100;

    while (count-- > 0) {
        linked_node_t* list = linked_list_create_empty();
        assert_true(list == NULL);

        int size = 100;
        int a[100];
        for (int i = 0; i < size; i++) {
            a[i] = rand();
            linked_list_preappend(&list, long2voidp(a[i]));
        }

        int i = 0;
        for (linked_node_t* it = list; it != NULL; it = LINKED_LIST_NEXT(it), i++) {
            assert_int_equal(voidp2long(it->data), a[size - i - 1]);
        }

        linked_list_destory(&list);
        assert_true(list == NULL);

        linked_list_append(&list, long2voidp(a[0]));
        linked_node_t* cur = list;
        for (int i = 1; i < size; i++) {
            linked_list_append(&cur, long2voidp(a[i]));
            cur = LINKED_LIST_NEXT(cur);
        }

        i = 0;
        for (linked_node_t* it = list; it != NULL; it = LINKED_LIST_NEXT(it), i++) {
            assert_int_equal(voidp2long(it->data), a[i]);
        }
        assert_int_equal(i, 100);

        linked_list_destory(&list);
        assert_true(list == NULL);
    }

}
