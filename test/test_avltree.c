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

static inline int max(int a, int b);

int int_cmp(const void* a, const void* b);
static int check_balance(avltree_node_t* root);
static void test(int* arr, int size);
static void test_remove(int* arr, int size);

void test_avltree(void **state)
{
    int size = 1000;
    test(range(0, 2 * size, 1), size);
    test(range(2 * size, 0, -1), size);
    test(rand_array(2 * size), size);

    test_remove(range(0, 2 * size, 1), size);
    test_remove(range(2 * size, 0, -1), size);
    test_remove(rand_array(2 * size), size);
}

static void test(int* arr, int size)
{
    avltree_t* tree = avltree_create(int_cmp);

    for (int i = 0; i < size; i++) {
        avltree_put(tree, long2voidp(arr[i]), long2voidp(arr[i]));
    }

    for (int i = 0; i < size; i++) {
        assert_true(avltree_exists(tree, long2voidp(arr[i])));
        assert_int_equal(voidp2long(avltree_get(tree, long2voidp(arr[i]), 0)), arr[i]);
    }

    for (int i = size; i < 2 * size; i++) {
        assert_false(avltree_exists(tree, long2voidp(arr[i])));
    }

    check_balance(tree->root);

    avltree_destory(tree);
}

static void test_remove(int* arr, int size)
{
    avltree_t* tree = avltree_create(int_cmp);

    for (int i = 0; i < 2 * size; i++) {
        avltree_put(tree, long2voidp(arr[i]), long2voidp(arr[i]));
    }

    for (int i = 0; i < size; i++) {
        avltree_remove(tree, long2voidp(arr[i]));
    }

    for (int i = 0; i < size; i++) {
        assert_false(avltree_exists(tree, long2voidp(arr[i])));
    }

    for (int i = size; i < 2 * size; i++) {
        assert_true(avltree_exists(tree, long2voidp(arr[i])));
        assert_int_equal(voidp2long(avltree_get(tree, long2voidp(arr[i]), 0)), arr[i]);
    }

    check_balance(tree->root);

    avltree_destory(tree);
}

static int check_balance(avltree_node_t* root)
{
    if (root == NULL) {
        return -1;
    }

    int left_height = check_balance(root->left);
    int right_height = check_balance(root->right);

    assert_int_equal(max(left_height, right_height) + 1, root->height);

    assert_true(abs(left_height - right_height) <= 1);

    return root->height;
}

int int_cmp(const void* a, const void* b)
{
    return voidp2long(a) - voidp2long(b);
}

static inline int max(int a, int b)
{
    return a > b ? a : b;
}
