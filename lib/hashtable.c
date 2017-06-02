#include "hashtable.h"

#include <string.h>
#include <assert.h>
#include "alloc.h"

#define INITIALIZE_BUCKET_SIZE 7
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
    size_t hash = table->hash_function(key, table->size);

    assert(hash < table->size);

    hashtable_bucket_t** head = table->buckets + hash;

    for (hashtable_bucket_t* it = *head; it != NULL; it = it->next) {
        if (table->cmp_function(key, it->key) == 0) {
            it->value = value;
            return;
        }
    }

    hashtable_bucket_t* node = FDS_NEW(hashtable_bucket_t, 1);
    node->key = key;
    node->value = value;

    node->next = *head;
    *head = node;

    table->len++;
}

bool hashtable_exists(hashtable_t* table, void* key)
{
    size_t hash = table->hash_function(key, table->size);

    assert(hash < table->size);

    for (hashtable_bucket_t* it = table->buckets[hash]; it != NULL; it = it->next) {
        if (table->cmp_function(key, it->key) == 0) {
            return true;
        }
    }
    return false;
}

void* hashtable_get(hashtable_t* table, void* key, void* default_value)
{
    size_t hash = table->hash_function(key, table->size);

    assert(hash < table->size);

    for (hashtable_bucket_t* it = table->buckets[hash]; it != NULL; it = it->next) {
        if (table->cmp_function(key, it->key) == 0) {
            return it->value;
        }
    }
    return default_value;
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
