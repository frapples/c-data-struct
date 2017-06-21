#ifndef __AVLTREE_H__RAND439874753491296604
#define __AVLTREE_H__RAND439874753491296604

#include <stdbool.h>
#include "utils.h"

typedef struct avltree_node_s {
    struct avltree_node_s* left;
    struct avltree_node_s* right;
    int height;
    void* key;
    void* value;
#ifndef NDEBUG
    int debug_height;
#endif // NDEBUG
}avltree_node_t;

typedef struct {
    avltree_node_t* root;
    CmpFunc cmp_function;
}avltree_t;

avltree_t* avltree_create(CmpFunc cmp_function);
avltree_t* avltree_create_strkey();

void avltree_put(avltree_t* tree, void* key, void* value);
bool avltree_exists(avltree_t* tree, void* key);
void* avltree_get(avltree_t* tree, void* key, void* default_value);
void avltree_remove(avltree_t* tree, void* key);

void avltree_destory(avltree_t* tree);


#endif /* __AVLTREE_H__RAND439874753491296604 */
