#include "fslist.h"
#include "uassert.h"

typedef struct fslist_node node_t;

size_t fslist_init( struct fslist* s, void* buff, size_t buffSize, size_t elemSize )
{
    size_t chunkSize = elemSize + sizeof( struct fslist_node );
    size_t it;
    size_t cap;

    s->buff = ( char*) buff;
    s->elemSize = elemSize;
    cap = buffSize / chunkSize;
    if ( cap > FSLIST_NUM_MAX_NODE )
    {
        uassert( false );
        return 0;
    }
    s->capacity = ( fslist_idx_t) ( cap );
    s->get = ( struct fslist_node* ) s->buff;
    s->data = s->buff + s->capacity * sizeof( struct fslist_node );
    
    s->head = s->tail = FSLIST_NODEIDX_NONE;
    s->inactive = 0;

    s->size = 0;

    // Link all nodes
    for ( it = 0; it < s->capacity; ++it )
    {
        s->get[it].next = ( fslist_idx_t) ( it + 1 );
        s->get[it].prev = ( fslist_idx_t) ( it - 1 );
        s->get[it].isValid = false;
    }
    s->get[s->capacity - 1].next = FSLIST_NODEIDX_NONE;

    return s->capacity;
}

struct fslist_node* fslist_insert( struct fslist* s, struct fslist_node* n )
{
    node_t* newNode;
    fslist_idx_t nidx, newNodeIdx;

    uassert( s->inactive != FSLIST_NODEIDX_NONE );
    uassert( n == NULL || fslist_node_in_range( s, n ) );
    uassert( n == NULL || n->isValid );

    // Allocate new node and insert before given node
    newNode = s->get + s->inactive;
    newNodeIdx = s->inactive;

    // Replace inactive node head.
    s->inactive = newNode->next;

    nidx = fslist_idx( s, n );
    newNode->next = nidx;

    // If pushing back ...
    if ( n == NULL )
    {
        if(s->tail != FSLIST_NODEIDX_NONE )
        {
            n = s->get + s->tail;
            n->next = newNodeIdx;
        }
        else
        {
            s->head = newNodeIdx; 
        }
        newNode->prev = s->tail;

        // Replace to new tail
        s->tail = newNodeIdx;
    }
    else
    {
        newNode->prev = n->prev;
        n->prev = newNodeIdx;

        if ( nidx == s->head )
            s->head = newNodeIdx;
        else
            s->get[newNode->prev].next = newNodeIdx;
    }

    newNode->isValid = true;
    ++s->size;
    return newNode;
}

void fslist_erase( struct fslist* s, struct fslist_node* n )
{
    fslist_idx_t nidx = fslist_idx( s, n );
    
    uassert( nidx != FSLIST_NODEIDX_NONE );
    uassert( s->size );
    uassert( n->isValid );

    // Unlink from active list
    if ( n->next != FSLIST_NODEIDX_NONE )
        s->get[n->next].prev = n->prev;
    else
        s->tail = n->prev;

    if ( n->prev != FSLIST_NODEIDX_NONE )
        s->get[n->prev].next = n->next;
    else
        s->head = n->next;

    // Link into available
    if ( s->inactive != FSLIST_NODEIDX_NONE )
        s->get[s->inactive].prev = nidx;
    n->next = s->inactive;
    s->inactive = nidx;
    
    n->isValid = false;
    --s->size;
}