#include "fslist.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#define assert_idx(idx) assert((idx) < list->poolCapacity)
static inline void unlink( fslist_t* list, fslistNodeIdx_t a )
{
    assert_idx( a );
    fslistNode_t* n = list->pool + a;
    
    if ( n->next != fslist_nodeidx_none ) 
        list->pool[n->next].prev = n->prev; 

    if ( n->prev != fslist_nodeidx_none ) 
        list->pool[n->prev].next = n->next;  

    n->next = fslist_nodeidx_none;
    n->prev = fslist_nodeidx_none;
}

static inline void link_after( fslist_t* list, fslistNodeIdx_t base, fslistNodeIdx_t node )
{
    assert_idx( base );
    assert_idx( node );
    fslistNode_t* b = list->pool + base;
    fslistNode_t* n = list->pool + node;

    unlink( list, node );
    
    if ( b->next != fslist_nodeidx_none ) 
        list->pool[b->next].prev = node; 

    n->next = b->next;
    n->prev = base;

    b->next = node;
}

static inline void link_before( fslist_t* list, fslistNodeIdx_t base, fslistNodeIdx_t node )
{
    assert_idx( base );
    assert_idx( node );
    fslistNode_t* b = list->pool + base;
    fslistNode_t* n = list->pool + node;

    unlink( list, node );
    
    if ( b->prev != fslist_nodeidx_none ) 
        list->pool[b->prev].next = node;  

    n->prev = b->prev;
    n->next = base;

    b->prev = node;
}

static inline void alloc_data( fslist_t* list, void const* initData, fslistNodeIdx_t target )
{
    assert_idx( target );
    assert( list->pool[target].pdata == NULL );

    void* ptr = list->dataPool + list->elemSize * target;
    list->pool[target].pdata = ptr; 
    if ( list->ctorCallback )
        list->ctorCallback( ptr, initData );
    else if ( initData )
        memcpy( ptr, initData, list->elemSize );
}

static inline void free_data( fslist_t* list, fslistNodeIdx_t target )
{
    assert_idx( target );
    assert( list->pool[target].pdata == NULL );

    void* ptr = list->dataPool + list->elemSize * target;
    list->pool[target].pdata = NULL;
    list->dtorCallback( ptr );
}

fslistNodeIdx_t fslistGetOffset( fslist_t const* list, fslistNode_t const* node )
{
    if ( node < list->pool || node >= list->pool + list->poolCapacity ) 
        return fslist_nodeidx_none; 
    else
        return ( node - list->pool ) / sizeof( fslistNode_t );
}

static void nulldtor( void* placeholder ) {}

bool fslistInit( fslist_t* list, size_t initCapacity, size_t elementSize )
{
    list->pool = ( fslistNode_t*) malloc( initCapacity * sizeof( fslistNode_t ) );
    list->dataPool = ( fslistNode_t*) malloc( initCapacity * elementSize );

    // Memory allcoation has failed ...
    if ( ( list->pool && list->dataPool ) == false ) 
        return false; 

    list->elemSize = elementSize;
    list->poolCapacity = initCapacity;

    list->ctorCallback = NULL;
    list->dtorCallback = nulldtor;

    list->availableHead = 0;
    list->head = list->tail = fslist_nodeidx_none;

    // Link all available nodes 
    // This process is to make searching available node easier.
    {
        fslistNode_t* pool = list->pool;
        fslistNodeIdx_t head = 0;

        // the first and last node need to be initialized separately.
        pool[0].prev = fslist_nodeidx_none;
        pool[0].pdata = NULL;
        pool[list->poolCapacity - 1].next = fslist_nodeidx_none;

        // Links nodes iteratively.
        while ( ++head < initCapacity )
        {
            pool[head - 1].next = head; 
            pool[head].prev = head - 1;
            pool[head].pdata = NULL;
        } 
    }
    return true;
}

fslistNode_t* fslistInsertNew( fslist_t* list, fslistNodeIdx_t node, void const* initData )
{
    assert_idx( node );

    fslistNodeIdx_t gen = list->availableHead;
    list->availableHead = list->pool[gen].next;
    unlink( list, gen );
    // Mandates handling initdata to ctor.
    alloc_data( list, gen, initData );

    // If the case inserting to front.
    if ( node == fslist_nodeidx_none )
    {
        if ( list->head != fslist_nodeidx_none )
            link_before( list, list->head, gen );
        else // head is empty. Means tail is empty either.
            list->tail = gen;

        list->head = gen;
    }
    else
    {
        assert( list->pool[node].pdata == NULL );
        if ( list->pool[node].pdata == NULL )
            return NULL;

        link_after( list, node, gen );

        if ( list->tail == node )
            list->tail = gen;
    }

    return list->pool + gen;
}

bool fslistErase( fslist_t* list, fslistNodeIdx_t* node )
{
    return false;
}
