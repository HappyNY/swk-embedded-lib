/*! \brief Implements dynamic allocatible queue. */
#pragma once
#include <stdlib.h>
#include "allocator.h"

struct queue_allocator
{
    allocator_ref_t allocator;
    size_t head;
    size_t tail;
    size_t cap;
    size_t cnt;
    char* buff;
};


/*! \brief Initialize queue allcoator. */
void queue_allocator_init( struct queue_allocator* s, allocator_ref_t alloc, size_t capacity );

/*! \brief Destroy allocator */
void queue_allocator_destroy( struct queue_allocator* s );

/*! \brief Push new data into queue. 
    \details 
        First queue's head indexer will move firstly, then the data will be returned. 
        Internal data will be aligned by 4-byte order. */
void* queue_allocator_push( struct queue_allocator* s, size_t size );

/*! \brief Pop data from queue. Not returns popped data. */
void queue_allocator_pop( struct queue_allocator* s );

/*! \brief Peek next data which pending to be popped on next pop() call. Data size will also be returned. */
void* queue_allocator_peek( struct queue_allocator* s, size_t* size );
