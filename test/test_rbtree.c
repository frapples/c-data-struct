#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "google/cmockery.h"

#include "voidp.h"
#include "rbtree.h"
#include "./utils.h"


static void test(int* arr, int size);

void test_rbtree(void **state)
{
    int size = 1000;
    test(range(0, 2 * size, 1), size);
    test(range(2 * size, 0, -1), size);
    test(rand_array(2 * size), size);
}

static void test(int* arr, int size)
{
    rbtree_t* tree = rbtree_create(int_cmp);

    for (int i = 0; i < size; i++) {
        rbtree_put(tree, long2voidp(arr[i]), long2voidp(arr[i]));
    }

    for (int i = 0; i < size; i++) {
        printf("%d\n", arr[i]);
        assert_true(rbtree_exists(tree, long2voidp(arr[i])));
        assert_int_equal(voidp2long(rbtree_get(tree, long2voidp(arr[i]), 0)), arr[i]);
    }

    for (int i = size; i < 2 * size; i++) {
        assert_false(rbtree_exists(tree, long2voidp(arr[i])));
    }

    assert_true(rbtree_check_struct(tree));

    rbtree_destory(tree);
}
