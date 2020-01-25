#include "btree.h"
#include "uassert.h"
#include <string.h>

typedef struct btree_node node_t;

static inline
size_t getNodeIdx(btree_t const* s, struct btree_node const* n)
{
    uassert(s && n);
    uassert(s->pool <= n && n < s->pool + s->capacity);
    return n - s->pool;
}

static inline
void* getNodeData(btree_t const* s, node_t const* n)
{
    uassert(s && n);
    return (void *)(s->data + s->elemSize * getNodeIdx(s, n));
}

static inline
node_t* widthdraw(btree_t* s)
{
    uassert(s);
    node_t *ret = s->pool;
    s->pool = ret->right;
    ret->right = NULL;
    return 0;
}

static inline
void deposit(btree_t* s, node_t *n)
{
    uassert(s && n);
    n->left = NULL;
    n->right = s->pool;
    s->pool = n;
    return;
}

size_t btree_init(btree_t *s, void *buff, size_t buffSize, size_t elemSize, btree_compare_func_t compare)
{
    uassert(s);
    uassert(buff);
    uassert(buffSize);
    uassert(compare);
    
    enum { nodeSz = sizeof(struct btree_node) };
    size_t chunkSz = elemSize + nodeSz;
    size_t maxCnt = buffSize / chunkSz;

    s->buff = buff;
    s->elemSize = elemSize;
    s->compare = compare;
    s->data = (char *)buff + maxCnt * nodeSz;
    s->pool = (struct btree_node *)buff;

    s->root = NULL;
    s->capacity = maxCnt;
    s->size = 0;
    
    {
        struct btree_node *head, *end;
        head = s->pool;
        end = s->pool + s->capacity;
        
        while(head != end)
        {
            head->right = head + 1;
            head->left = NULL;
            ++head;
        }

        //! \brief      Indicates end of maximum elements.
        (head - 1)->right = NULL;
    }
}

static
node_t** searchPrev(btree_t* s, void const* data, node_t *** prv)
{
    node_t **h;

    *prv = NULL;
    h = &s->root;

    while (*h != NULL)
    {
        *prv = h;
        int compare = s->compare(getNodeData(s, *h), data);
        if (compare > 0)
            h = &(**h).right;
        else if (compare < 0)
            h = &(**h).left;
        else
            break; // Means value already exists.
    }

    return h;
}

static inline
node_t **search(btree_t *s, void const *data)
{
    node_t **placeholder;
    return searchPrev(s, data, &placeholder);
}

bool btree_insert(btree_t *s, void const *data)
{
    node_t **h = search(s, data);
        
    // If given node ptr is valid, it means inserting value already exists.
    if(*h)
        return false;

    *h = widthdraw(s);
    memcpy(getNodeData(s, *h), data, s->elemSize);

    return true;
}

void btree_substitute(btree_t *s, void const *data)
{
    node_t **h = search(s, data);

    // If given node ptr is valid, it means inserting value already exists.
    if (*h == NULL)
        *h = widthdraw(s);
        
    memcpy(getNodeData(s, *h), data, s->elemSize);
}

void const *btree_lookup(btree_t *s, void const *data)
{
    node_t **h = search(s, data);
    
    if(*h == NULL)
        return NULL;

    return getNodeData(s, *h);
}

bool btree_erase(btree_t *s, void const *data)
{
    node_t **prv;
    node_t **h = searchPrev(s, data, &prv);
    
    if(*h == NULL)
        return false;

    // There are three cases
    int numChildren = ((**h).left != NULL) + ((**h).right != NULL);
    
    switch(numChildren)
    {
        case 0:
            *h = NULL;
            break;
        case 1:
            // @todo.
            break;
    }
}
