#include "managed_object_pool.h"
#include "uassert.h"
#include <stdio.h>

typedef struct objnode
{
    void *ref;
    uint32_t id;
    int16_t lockcnt;
    bool pending_free;
} objnode_t;

struct managed_object_pool
{
    /*data*/
    uint32_t idgen;
    struct fslist refs;
};

managed_object_pool_t* objpool_init(size_t numMaxRef)
{
    managed_object_pool_t* s;
    uassert(s);
    size_t buffSz = numMaxRef * (FSLIST_NODE_SIZE + sizeof(objnode_t));

    s->idgen = 0;
    fslist_init(&s->refs, malloc(buffSz), buffSz, sizeof(objnode_t));

    uassert(&s->refs.capacity == numMaxRef);

    return s;
}

objhandle_t objpool_malloc(managed_object_pool_t *s, size_t memsize)
{
    uassert(s && s->refs.size < s->refs.capacity);

    struct fslist_node *n = fslist_insert(&s->refs, NULL);
    objnode_t *data = fslist_data(&s->refs, n);
    uassert(n && data);

    data->id = s->idgen++;
    data->pending_free = false;
    data->ref = malloc(memsize);
    data->lockcnt = 0;

    objhandle_t ret;
    ret.id = data->id;
    ret.node = n;
    ret.s = s;
    return ret;
}

void objpool_foreach(managed_object_pool_t *s, void *caller, objpool_foreach_callback_t cb)
{
    uassert(s && cb);

    if ( s->refs.size == 0 )
        return;

    // First node ref
    struct fslist_node* n = &s->refs.get[s->refs.head]; 
    objnode_t* objref;
    objhandle_t h;
    h.s = s;

    while ( n )
    {
        objref = fslist_data(&s->refs, n);
        
        // Make handle from node
        h.node = n;
        h.id = objref->id;

        // Lock node to prevent iteration breakdown
        if ( obj_lock(&h) )
        {
            // Callback.
            cb(caller, &h);

            n = fslist_next(&s->refs, n);
            obj_unlock(&h);
        }
        else
        {
            n = fslist_next(&s->refs, n);
        }
    }
}

void *obj_lock(objhandle_t const *h)
{
    uassert(h->s && h->id != OBJECTID_NULL);
    objnode_t *data = fslist_data(&h->s->refs, h->node);

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

void obj_unlock(objhandle_t const *h)
{
    uassert(h->s && h->id != OBJECTID_NULL);
    objnode_t *data = fslist_data(&h->s->refs, h->node);

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
            fslist_erase(h->s, &h->node);
        }
    }
}

bool obj_free(objhandle_t const *h)
{
    uassert(h->s && h->id != OBJECTID_NULL);
    objnode_t *data = fslist_data(&h->s->refs, h->node);

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
        fslist_erase(h->s, &h->node);
        return true;
    }
    else
    {
        return false;
    }
}

bool obj_is_valid(objhandle_t const* h)
{
    objnode_t* data = fslist_data(&h->s->refs, h->node);

    if ( data && data->id == h->id && data->pending_free == false )
    {
        return true;
    }
    else
    {
        return false;
    }
}