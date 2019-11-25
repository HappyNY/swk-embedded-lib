/*! \brief
    \file managed_reference_pool_t.h
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
    struct managed_reference_pool** s;
    struct fslist_node const* node;
    uint32_t id;
} refhandle_t;

typedef struct managed_reference_pool managed_reference_pool_t;

managed_reference_pool_t* refpool_create(size_t numMaxRef);
refhandle_t refpool_malloc(managed_reference_pool_t *s, size_t memsize);
size_t refpool_num_available(managed_reference_pool_t* s);

//! \warning    This function invalidates all alive reference handles!
void refpool_destroy(managed_reference_pool_t* s);

typedef void (*refpool_foreach_callback_t)(void */*caller*/, refhandle_t const */*iter_object*/);
void refpool_foreach(managed_reference_pool_t *s, void *caller, refpool_foreach_callback_t cb);

bool ref_free(refhandle_t const *h);
void *ref_lock(refhandle_t const *h);
void ref_unlock(refhandle_t const *h);
bool ref_is_valid(refhandle_t const* h);
