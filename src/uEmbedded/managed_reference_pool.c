#include "managed_reference_pool.h"
#include "uassert.h"
#include <stdio.h>

typedef struct refnode
{
    void *ref;
    uint32_t id;
    int16_t lockcnt;
    bool pending_free;
} refnode_t;

struct managed_reference_pool
{
    /*data*/
    struct managed_reference_pool* ref_to_myself;
    uint32_t idgen;
    struct fslist refs;
};

managed_reference_pool_t* refpool_create(size_t numMaxRef)
{
    managed_reference_pool_t* s = malloc(sizeof(managed_reference_pool_t));
    if ( s == NULL )
        return NULL;

    size_t buffSz = numMaxRef * (FSLIST_NODE_SIZE + sizeof(refnode_t));

    s->idgen = 0;
    fslist_init(&s->refs, malloc(buffSz), buffSz, sizeof(refnode_t));

    uassert(s->refs.capacity == numMaxRef);
    s->ref_to_myself = s;
    return s;
}

refhandle_t refpool_malloc(managed_reference_pool_t *s, size_t memsize)
{
    uassert(s && s->refs.size < s->refs.capacity);

    struct fslist_node *n = fslist_insert(&s->refs, NULL);
    refnode_t *data = fslist_data(&s->refs, n);
    uassert(n && data);

    data->id = s->idgen++;
    data->pending_free = false;
    data->ref = malloc(memsize);
    data->lockcnt = 0;

    refhandle_t ret;
    ret.id = data->id;
    ret.node = n;
    ret.s = s;
    return ret;
}

size_t refpool_num_available(managed_reference_pool_t* s)
{
    return s->refs.capacity - s->refs.size;
}

static void release_all(void* nouse_, refhandle_t* h)
{
    free(ref_lock(h));
}

void refpool_destroy(managed_reference_pool_t* s)
{
    // Release all memory
    refpool_foreach(s, NULL, release_all);

    // Releaes ref to myself
    s->ref_to_myself = NULL;

    // Erase ref to myself
    free(s);
}

void refpool_foreach(managed_reference_pool_t *s, void *caller, refpool_foreach_callback_t cb)
{
    uassert(s && cb);

    if ( s->refs.size == 0 )
        return;

    // First node ref
    struct fslist_node* n = &s->refs.get[s->refs.head]; 
    refnode_t* objref;
    refhandle_t h;
    h.s = s;

    while ( n )
    {
        objref = fslist_data(&s->refs, n);
        
        // Make handle from node
        h.node = n;
        h.id = objref->id;

        // Lock node to prevent iteration breakdown
        if ( ref_lock(&h) )
        {
            // Callback.
            cb(caller, &h);

            n = fslist_next(&s->refs, n);
            ref_unlock(&h);
        }
        else
        {
            n = fslist_next(&s->refs, n);
        }
    }
}

void *ref_lock(refhandle_t const *h)
{
    uassert(h->s && h->id != OBJECTID_NULL);

    // Check if reference is alive.
    managed_reference_pool_t* s = *h->s;
    if ( s == NULL )
        return NULL;

    refnode_t *data = fslist_data(&s->refs, h->node);

    if (data && data->id == h->id && data->pending_free == false)
    {
        uassert(data->ref);
        ++data->lockcnt;
        return data->ref;
    }
    else
    {
        return NULL;
    }
}

void ref_unlock(refhandle_t const *h)
{
    uassert(h->s && h->id != OBJECTID_NULL);

    // Check if reference is alive.
    managed_reference_pool_t* s = *h->s;
    if ( s == NULL )
        return NULL;

    refnode_t *data = fslist_data(&s->refs, h->node);

    if (data && data->id == h->id)
    {
        uassert(data->lockcnt > 0);
        --data->lockcnt;

        // Erase node
        if (data->pending_free && data->lockcnt == 0)
        {
            uassert(data->ref);
            free(data->ref);
            data->id = OBJECTID_NULL;
            fslist_erase(&s->refs, h->node);
        }
    }
}

bool ref_free(refhandle_t const *h)
{
    uassert(h->s && h->id != OBJECTID_NULL);

    // Check if reference is alive.
    managed_reference_pool_t* s = *h->s;
    if ( s == NULL )
        return NULL;

    refnode_t *data = fslist_data(&s->refs, h->node);

    if (data && data->id == h->id)
    {
        if (data->lockcnt)
        {
            data->pending_free = true;
            return true;
        }

        // Erase node
        free(data->ref);
        data->id = OBJECTID_NULL;
        fslist_erase(&s->refs, h->node);
        return true;
    }
    else
    {
        return false;
    }
}

bool ref_is_valid(refhandle_t const* h)
{
    // Check if reference is alive.
    managed_reference_pool_t* s = *h->s;
    if ( s == NULL )
        return false;

    refnode_t* data = fslist_data(&s->refs, h->node);

    if ( data && data->id == h->id && data->pending_free == false )
    {
        return true;
    }
    else
    {
        return false;
    }
}