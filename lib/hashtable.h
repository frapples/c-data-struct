#ifndef __HASHTABLE_H__RAND6950493206809747
#define __HASHTABLE_H__RAND6950493206809747

#include <stddef.h>
#include <stdbool.h>

typedef struct hashtable_bucket_s{
    struct hashtable_bucket_s* next;
    void* key;
    void* value;
} hashtable_bucket_t;

typedef size_t (*HashFunc)(const void *, size_t);
typedef int (*CmpFunc)(const void *, const void*);

typedef struct {
    size_t size;
    size_t len;
    HashFunc hash_function;
    CmpFunc cmp_function;
    hashtable_bucket_t** buckets;
} hashtable_t;

typedef void (*HashtableEachCallback)(void* key, void* value, void* data);

hashtable_t* hashtable_create(HashFunc hash_function, CmpFunc cmp_function);
hashtable_t* hashtable_create_strkey();
void hashtable_put(hashtable_t* table, void* key, void* value);
bool hashtable_exists(hashtable_t* table, void* key);
void* hashtable_get(hashtable_t* table, void* key, void* default_value);
void hashtable_remove(hashtable_t* table, void* key);

size_t hashtable_len(hashtable_t* table);

void hashtable_each(hashtable_t* table, HashtableEachCallback callback, void* callback_data);

size_t hashtable_str_hash(const void* str, size_t table_size);


#endif /* __HASHTABLE_H__RAND6950493206809747 */
