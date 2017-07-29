#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "google/cmockery.h"

#include "voidp.h"
#include "skiplist.h"
#include "./utils.h"


static void test(int* arr, int size);

void test_skiplist(void **state)
{
    int size = 1000;
    test(range(0, 2 * size, 1), size);
    test(range(2 * size, 0, -1), size);
    test(rand_array(2 * size), size);
}

static void test(int* arr, int size)
{
    skiplist_t* list = skiplist_create(int_cmp);

    for (int i = 0; i < size; i++) {
        skiplist_put(list, long2voidp(arr[i]), long2voidp(arr[i]));
    }

    for (int i = 0; i < size; i++) {
        assert_true(skiplist_exists(list, long2voidp(arr[i])));
        assert_int_equal(voidp2long(skiplist_get(list, long2voidp(arr[i]), 0)), arr[i]);
    }

    for (int i = size; i < 2 * size; i++) {
        assert_false(skiplist_exists(list, long2voidp(arr[i])));
    }

    skiplist_destory(list);
}
