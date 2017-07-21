#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "google/cmockery.h"

#include "voidp.h"
#include "priorityqueue.h"
#include "./utils.h"


int qsort_int_cmp(const void* p_a, const void* p_b)
{
    return *((int *)p_a) - *((int *)p_b);
}

void test_priorityqueue(void **state)
{
    int size = 1000;
    int* arr = rand_array(size);
    priority_queue_t* queue = priority_queue_create(int_cmp);
    for (int i = 0; i < size; i++) {
        priority_queue_insert(queue, long2voidp(arr[i]));
    }

    for (int i = 0; i < size; i++) {
        int n = voidp2long(*ARRAYLIST_GET(queue->array, i, void *));
        assert_true(int_is_in(arr, size, n));
    }

    for (int i = 0; i < size; i++) {
        bool finded = false;
        for (int j = 0; j < size; j++) {
            int n = voidp2long(*ARRAYLIST_GET(queue->array, j, void *));
            if (n == arr[i]) {
                finded = true;
            }
        }
        assert_true(finded);
    }

    qsort(arr, size, sizeof(arr[0]), qsort_int_cmp);
    for (int i = 0; i < size; i++) {
        int len = size - i;
        assert_int_equal(priority_queue_len(queue), len);

        assert_int_equal(voidp2long(priority_queue_get_min(queue)), arr[i]);
        assert_int_equal(voidp2long(priority_queue_delete_min(queue)), arr[i]);
    }
}

