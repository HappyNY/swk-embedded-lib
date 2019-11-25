/*! \brief
    \file managed_reference_pool.h
    \author Seungwoo Kang (ki6080@gmail.com)
    \version 0.1
    \date 2019-11-24
    \copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.
    
    \details
      objhandle ---> objnode ---> actual reference
 */
#pragma once
#include "fslist.h"
#include <stdlib.h>
#include <stdint.h>

enum
{
    OBJECTID_NULL = -1u
};

typedef struct refhandle
{
    managed_reference_pool *s;
    struct fslist_node const *node;
    uint32_t id;
} refhandle_t;

typedef struct managed_reference_pool managed_reference_pool;

managed_reference_pool* objpool_init(size_t numMaxRef);
refhandle_t objpool_malloc(managed_reference_pool *s, size_t memsize);

typedef void (*objpool_foreach_callback_t)(void *caller, refhandle_t const *iter_object);
void objpool_foreach(managed_reference_pool *s, void *caller, objpool_foreach_callback_t cb);

bool obj_free(refhandle_t const *h);
void *obj_lock(refhandle_t const *h);
void obj_unlock(refhandle_t const *h);
bool obj_is_valid(refhandle_t const* h);
