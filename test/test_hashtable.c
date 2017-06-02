#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include "google/cmockery.h"

#include "hashtable.h"
#include "./utils.h"

static void each_callback(void* key, void* value, void* data);

struct each_callback_data_s {
    char** strs;
    int size;
};

void test_hashtable(void **state)
{
    int size = 1000;
    hashtable_t* table = hashtable_create_strkey();
    char** strs = rand_str_array(size + size);
    for (int i = 0; i < size; i+= 2) {
        hashtable_put(table, strs[i], strs[i + 1]);
    }

    assert_int_equal(hashtable_len(table), size / 2);

    /* 填入存在的数据 */
    for (int i = 0; i < size; i+= 2) {
        hashtable_put(table, strs[i], strs[i + 1]);
    }
    assert_int_equal(hashtable_len(table), size / 2);

    /* 取存在的数据 */
    for (int i = 0; i < size; i+= 2) {
        assert_true(hashtable_exists(table, strs[i]));
        assert_string_equal(hashtable_get(table, strs[i], NULL), strs[i + 1]);
    }

    /* 取不存在的数据 */
    for (int i = 0; i < size; i+= 2) {
        char** s = strs + size;
        assert_false(hashtable_exists(table, s[i]));
        assert_true(hashtable_get(table, s[i], NULL) == NULL);
    }

    hashtable_each(table, each_callback, &(struct each_callback_data_s){strs, size});


    /* 删除数据 */
    for (int i = 0; i < size; i+= 2) {
        hashtable_remove(table, strs[i]);
    }
    assert_int_equal(hashtable_len(table), 0);

    for (size_t i = 0; i < table->size; i++) {
        assert_true(table->buckets[i] == NULL);
    }

    /* 测试 hashtable_destory */
    for (int i = 0; i < size; i+= 2) {
        hashtable_put(table, strs[i], strs[i + 1]);
    }
    hashtable_destory(table);
}

static void each_callback(void* key, void* value, void* data_)
{
    struct each_callback_data_s* data = data_;

    assert_true(str_is_in(data->strs, data->size, key));
    assert_true(str_is_in(data->strs, data->size, value));
}
