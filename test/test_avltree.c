#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "google/cmockery.h"

#include "voidp.h"
#include "avltree.h"
#include "./utils.h"

int int_cmp(const void* a, const void* b);

void test_avltree(void **state)
{
    int size = 1000;

    avltree_t* tree = avltree_create(int_cmp);

    for (int i = 0; i < size; i++) {
        avltree_put(tree, long2voidp(i), long2voidp(i));
    }

    for (int i = 0; i < size; i++) {
        assert_true(avltree_exists(tree, long2voidp(i)));
        assert_int_equal(voidp2long(avltree_get(tree, long2voidp(i), 0)), i);
    }

    for (int i = size; i < 2 * size; i++) {
        assert_false(avltree_exists(tree, long2voidp(i)));
    }
}

int int_cmp(const void* a, const void* b)
{
    return voidp2long(a) - voidp2long(b);
}
