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

void *objpool_lock(managed_object_pool_t *s, objhandle_t h)
{
    uassert(s && h.id != OBJECTID_NULL);
    objnode_t *data = fslist_data(&s->refpool, h.node);

    if (data && data->id == h.id && data->pending_free == false)
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

void objpool_unlock(managed_object_pool_t *s, objhandle_t h)
{
    uassert(s && h.id != OBJECTID_NULL);
    objnode_t *data = fslist_data(&s->refpool, h.node);

    if (data && data->id == h.id)
    {
        uassert(data->lockcnt > 0);
        --data->lockcnt;

        // Erase node
        if (data->pending_free && data->lockcnt == 0)
        {
            uassert(data->ref);
            free(data->ref);
            data->id = OBJECTID_NULL;
            fslist_erase(s, &h.node);
        }
    }
}

bool objpool_free(managed_object_pool_t *s, objhandle_t h)
{
    uassert(s && h.id != OBJECTID_NULL);
    objnode_t *data = fslist_data(&s->refpool, h.node);

    if (data && data->id == h.id)
    {
        if (data->lockcnt)
        {
            data->pending_free = true;
            return true;
        }

        // Erase node
        free(data->ref);
        data->id = OBJECTID_NULL;
        fslist_erase(s, &h.node);
        return true;
    }
    else
    {
        return false;
    }
}