#ifndef __HASHTABLE_H__RAND6950493206809747
#define __HASHTABLE_H__RAND6950493206809747

#include <stddef.h>
#include <stdbool.h>

typedef struct hashtable_bucket_s{
    struct hashtable_bucket_s* next;
    void* key;
    void* value;
} hastable_bucket_t;

typedef size_t (*HashFunc)(void *, size_t);

typedef struct {
    size_t size;
    HashFunc hash_function;
    hastable_bucket_t* buckets;
} hashtable_t;

typedef void (*HashtableEachCallback)(void* key, void* value, void* data);

hashtable_t* hashtable_create(HashFunc hash_function);
hashtable_t* hashtable_create_strkey();
void hashtable_put(hashtable_t* table, void* key, void* value);
bool hashtable_exists(hashtable_t* table, void* key);
void* hashtable_get(hashtable_t* table, void* key, void* default_value);

size_t hashtable_len(hashtable_t* table);

void hashtable_each(hashtable_t* table, HashtableEachCallback callback);


#endif /* __HASHTABLE_H__RAND6950493206809747 */
