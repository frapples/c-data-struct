#include "hashtable.h"

#include <string.h>
#include <assert.h>
#include "alloc.h"

#define INITIALIZE_BUCKET_SIZE 7
#define LOAD_FACTOR 0.75

static hashtable_bucket_t** hashtable_find(hashtable_t* table, void* key);
static void hashtable_try_rehash(hashtable_t* table);

hashtable_t* hashtable_create(HashFunc hash_function, CmpFunc cmp_function)
{
    hashtable_t* table = FDS_NEW(hashtable_t, 1);
    table->size = INITIALIZE_BUCKET_SIZE;
    table->len = 0;
    table->hash_function = hash_function;
    table->cmp_function = cmp_function;
    table->buckets = FDS_NEW(hashtable_bucket_t*, table->size);
    for (size_t i = 0; i < table->size; i++) {
        table->buckets[i] = NULL;
    }
    return table;
}

hashtable_t* hashtable_create_strkey()
{
    return hashtable_create(hashtable_str_hash, (CmpFunc)strcmp);
}

void hashtable_put(hashtable_t* table, void* key, void* value)
{
    hashtable_bucket_t** exists_node = hashtable_find(table, key);
    if (exists_node != NULL) {
        (*exists_node)->value = value;
    } else {
        size_t hash = table->hash_function(key, table->size);

        assert(hash < table->size);

        hashtable_bucket_t** head = table->buckets + hash;
        hashtable_bucket_t* node = FDS_NEW(hashtable_bucket_t, 1);
        node->key = key;
        node->value = value;

        node->next = *head;
        *head = node;

        table->len++;

        hashtable_try_rehash(table);
    }
}

bool hashtable_exists(hashtable_t* table, void* key)
{
    return hashtable_find(table, key) != NULL;
}

void* hashtable_get(hashtable_t* table, void* key, void* default_value)
{
    hashtable_bucket_t** node = hashtable_find(table, key);
    return node == NULL ? default_value : (*node)->value;
}

void hashtable_remove(hashtable_t* table, void* key)
{
    hashtable_bucket_t** p_node = hashtable_find(table, key);
    if (p_node != NULL) {
        hashtable_bucket_t* old = *p_node;
        *p_node = (*p_node)->next;
        fds_free(old);
        table->len--;
    }
}

static hashtable_bucket_t** hashtable_find(hashtable_t* table, void* key)
{
    size_t hash = table->hash_function(key, table->size);

    assert(hash < table->size);

    for (hashtable_bucket_t** it = &table->buckets[hash]; *it != NULL; it = &((*it)->next)) {
        if (table->cmp_function(key, (*it)->key) == 0) {
            return it;
        }
    }
    return NULL;
}

size_t hashtable_len(hashtable_t* table)
{
    return table->len;
}

void hashtable_each(hashtable_t* table, HashtableEachCallback callback, void* callback_data)
{
#ifndef NDEBUG
    size_t len = 0;
#endif // NDEBUG

    for (size_t i = 0; i < table->size; i++) {
        hashtable_bucket_t* head = table->buckets[i];
        for (hashtable_bucket_t* it = head; it != NULL; it = it->next) {
            (*callback)(it->key, it->value, callback_data);

#ifndef NDEBUG
            len++;
#endif // NDEBUG
        }
    }

    assert(len == table->len);
}

void hashtable_destory(hashtable_t* table)
{
    for (size_t i = 0; i < table->size; i++) {
        hashtable_bucket_t* cur = table->buckets[i];
        while (cur != NULL) {
            hashtable_bucket_t* next = cur->next;
            fds_free(cur);
            cur = next;
        }
    }

    fds_free(table->buckets);
    fds_free(table);
}

size_t hashtable_str_hash(const void* str, size_t table_size)
{
    const char* s = (const char*)str;

    size_t hash = 0;
    while (*s != '\0') {
        /* 旧值乘以32加上字符的ASCII  */
        hash = (hash << 5) + *s;
        s++;
    }
    return hash % table_size;
}

static void hashtable_try_rehash(hashtable_t* table)
{
    if (table->len < (size_t)(table->size * LOAD_FACTOR)) {
        return;
    }

    size_t new_size = table->size * 2;
    hashtable_bucket_t** buckets = FDS_NEW(hashtable_bucket_t*, new_size);
    for (size_t i = 0; i < new_size; i++) {
        buckets[i] = NULL;
    }

    for (size_t i = 0; i < table->size; i++) {
        hashtable_bucket_t* cur = table->buckets[i];
        while (cur != NULL) {
            hashtable_bucket_t* next = cur->next;

            size_t hash = table->hash_function(cur->key, new_size);

            assert(hash < new_size);

            cur->next = buckets[hash];
            buckets[hash] = cur;

            cur = next;
        }
    }

    table->size = new_size;
    fds_free(table->buckets);
    table->buckets = buckets;
}
