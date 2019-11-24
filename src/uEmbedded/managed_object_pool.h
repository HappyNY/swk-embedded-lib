/*! \brief
    \file managed_object_pool.h
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

typedef struct objhandle
{
    struct fslist_node const *node;
    uint32_t id;
} objhandle_t;

typedef struct managed_object_pool
{
    /*data*/
    uint32_t idgen;
    struct fslist refpool;
} managed_object_pool_t;

void objpool_init(managed_object_pool_t *s, size_t numMaxRef);
objhandle_t objpool_malloc(managed_object_pool_t *s, size_t memsize);
bool objpool_free(managed_object_pool_t *s, objhandle_t h);
void *objpool_lock(managed_object_pool_t *s, objhandle_t h);
void objpool_unlock(managed_object_pool_t *s, objhandle_t h);
