#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "fslist.h"

#define assert_idx(idx) assert((idx) < list->poolCapacity)

static inline void unlink(fslist_t *list, fslist_idx_t a)
{
    assert_idx(a);
    fslist_node_t *n = list->pool + a;

    if (n->next != FSLIST_NODEIDX_NONE)
        list->pool[n->next].prev = n->prev;

    if (n->prev != FSLIST_NODEIDX_NONE)
        list->pool[n->prev].next = n->next;

    n->next = FSLIST_NODEIDX_NONE;
    n->prev = FSLIST_NODEIDX_NONE;
}

static inline void link_after(fslist_t *list, fslist_idx_t base, fslist_idx_t node)
{
    assert_idx(base);
    assert_idx(node);
    fslist_node_t *b = list->pool + base;
    fslist_node_t *n = list->pool + node;

    unlink(list, node);

    if (b->next != FSLIST_NODEIDX_NONE)
        list->pool[b->next].prev = node;

    n->next = b->next;
    n->prev = base;

    b->next = node;
}

static inline void link_before(fslist_t *list, fslist_idx_t base, fslist_idx_t node)
{
    assert_idx(base);
    assert_idx(node);
    fslist_node_t *b = list->pool + base;
    fslist_node_t *n = list->pool + node;

    unlink(list, node);

    if (b->prev != FSLIST_NODEIDX_NONE)
        list->pool[b->prev].next = node;

    n->prev = b->prev;
    n->next = base;

    b->prev = node;
}

static inline void alloc_data(fslist_t *list, void const *initData, fslist_idx_t target)
{
    assert_idx(target);
    assert(list->pool[target].pdata == NULL);

    void *ptr = list->dataPool + list->elemSize * target;
    list->pool[target].pdata = ptr;
    if (list->ctorCallback)
        list->ctorCallback(ptr, initData);
    else if (initData)
        memcpy(ptr, initData, list->elemSize);
}

static inline void free_data(fslist_t *list, fslist_idx_t target)
{
    assert_idx(target);
    assert(list->pool[target].pdata != NULL);

    void *ptr = list->dataPool + list->elemSize * target;
    list->pool[target].pdata = NULL;
    list->dtorCallback(ptr);
}

fslist_idx_t fslist_getOffset(fslist_t const *list, fslist_node_t const *node)
{
    if (node < list->pool || node >= list->pool + list->poolCapacity)
        return FSLIST_NODEIDX_NONE;
    else
        return (fslist_idx_t)(node - list->pool);
}

static void nulldtor(void *placeholder) {}

bool fslist_init(fslist_t *list, allocator_ref_t alloca, size_t initCapacity, size_t elementSize)
{
    list->allocatorRef = alloca;

    list->pool = (fslist_node_t *) alloca->allocate(alloca->object, initCapacity * sizeof(fslist_node_t));
    list->dataPool = (uint8_t *) alloca->allocate( alloca->object, initCapacity * elementSize);

    // Memory allcoation has failed ...
    if ((list->pool && list->dataPool) == false)
        return false;

    list->elemSize = elementSize;
    list->poolCapacity = initCapacity;

    list->ctorCallback = NULL;
    list->dtorCallback = nulldtor;

    list->availableHead = 0;
    list->head = list->tail = FSLIST_NODEIDX_NONE;

    list->size = 0;

    // Link all available nodes
    // This process is to make searching available node easier.
    {
        fslist_node_t *pool = list->pool;
        fslist_idx_t head = 0;

        // the first and last node need to be initialized separately.
        pool[0].prev = FSLIST_NODEIDX_NONE;
        pool[0].pdata = NULL;
        pool[list->poolCapacity - 1].next = FSLIST_NODEIDX_NONE;

        // Links nodes iteratively.
        while (++head < initCapacity)
        {
            pool[head - 1].next = head;
            pool[head].prev = head - 1;
            pool[head].pdata = NULL;
        }
    }
    return true;
}

bool fslist_destroy(fslist_t *list)
{
    // Destroy all active nodes
    // Destructors will be automatically called.
    while (list->head != FSLIST_NODEIDX_NONE)
        if (!fslist_erase(list, list->head))
            return false;
    
    list->allocatorRef->release(list->allocatorRef->object, list->pool);
    list->allocatorRef->release(list->allocatorRef->object, list->dataPool);

    memset(list, 0, sizeof(*list));
    return true;
}

fslist_node_t *fslist_insertNew(fslist_t *list, fslist_idx_t node, void const *initData)
{
    fslist_idx_t gen = list->availableHead;
    list->availableHead = list->pool[gen].next;
    unlink(list, gen);
    // Mandates handling initdata to ctor.
    alloc_data(list, initData, gen);

    // If the case inserting to front.
    if (node == FSLIST_NODEIDX_NONE)
    {
        if (list->head != FSLIST_NODEIDX_NONE)
            link_before(list, list->head, gen);
        else // head is empty. Means tail is empty either.
            list->tail = gen;

        list->head = gen;
    }
    else
    {
        assert_idx(node);
        assert(list->pool[node].pdata == NULL);
        if (list->pool[node].pdata == NULL)
            return NULL;

        link_after(list, node, gen);

        if (list->tail == node)
            list->tail = gen;
    }

    ++list->size;
    return list->pool + gen;
}

bool fslist_erase(fslist_t *list, fslist_idx_t node)
{
    assert_idx(node);
    assert(list->pool[node].pdata != NULL);

    if (node == list->head)
        list->head = list->pool[node].next;

    if (node == list->tail)
        list->tail = list->pool[node].prev;

    fslist_idx_t avhead = list->availableHead;
    list->availableHead = node;
    link_before(list, avhead, node);

    free_data(list, node);

    --list->size;
    return true;
}
