#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include "uassert.h"

//! \breif      Priority queue struct. Based on minimum heap. 
//! \warning    Not thread-safe!
struct priority_queue
{
    //! \breif      Return negative value if a < b.
    int ( *pred )( void const* a, void const* b );

    //! \breif
    size_t elemSize;

    //! \breif
    size_t cnt;

    //! \breif
    size_t capacity;

    //! \brief  
    char* buff;
};

//! \breif      Alias for priority queue.
typedef struct priority_queue pqueue_t;

/*! \breif      Initialize new priority queue struct. */
void pqueue_init( struct priority_queue* s, size_t elemSize, void* buff, size_t buffSize, int ( *pred )( void const*, void const* ) );

/*! \breif      Push new element to queue. */
void pqueue_push( struct priority_queue* s, void const* elem );

/*! \breif      Pop element from queue. */
void pqueue_pop( struct priority_queue* s );

/*! \breif      Peak next element.  */
static inline void* pqueue_peek(struct priority_queue* s)
{
    uassert(s && s->cnt);
    return s->buff;
}