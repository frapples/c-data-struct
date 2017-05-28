#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "google/cmockery.h"

#include "voidp.h"
#include "arraylist.h"
#include "./utils.h"

void test_array_list(void** state)
{
    int size = 100;
    int* a = rand_array(size);

    int count = 100;
    while (count-- > 0) {
        arraylist_t* arr =  arraylist_create(sizeof(long));
        for (int i = 0; i < size; i++) {
            arraylist_append(arr, &a[i]);
        }

        assert_int_equal(arraylist_len(arr), size);

        for (int i = 0; i < size; i++) {
            assert_int_equal(*ARRAYLIST_GET(arr, i, int), a[i]);
        }

        int start = rand_int(0, arraylist_len(arr));
        int end = rand_int(start, arraylist_len(arr));
        arraylist_remove(arr, start, end);
        assert_int_equal(arraylist_len(arr), size  - (end - start));

        int i = 0;
        for (int j = 0; j < start; j++) {
            assert_int_equal(*ARRAYLIST_GET(arr, i, int), a[j]);
            i++;
        }
        for (int j = end; j < size; j++) {
            assert_int_equal(*ARRAYLIST_GET(arr, i, int), a[j]);
            i++;
        }
        arraylist_destory(arr);
    }
}
