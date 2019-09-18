#include "queue_allocator.h"
#include <assert.h>
#include <stdbool.h>

void queue_allocator_init( struct queue_allocator* s, allocator_ref_t alloc, size_t capacity )
{
    s->allocator = alloc;
    capacity = ( capacity + sizeof( size_t ) - 1 ) & ~( sizeof( size_t ) - 1 );
    s->buff = s->allocator->allocate( s->allocator->object, capacity );
    s->head = 0;
    s->tail = 0;
    s->cnt = 0;
    s->cap = capacity;
}

void queue_allocator_destroy( struct queue_allocator* s )
{
    s->allocator->release( s->allocator->object, s->buff );
}

void* queue_allocator_push( struct queue_allocator* s, size_t size )
{
    // Memory is on 4-byte alignment.
    size_t jmpSize = sizeof( size_t ) + ( ( size + ( sizeof( size_t ) - 1 ) ) & ~( sizeof( size_t ) - 1 ) );

    if ( s->head + jmpSize + sizeof( size_t ) >= s->cap )
    {
        // Notifies the position to go back.
        *( size_t*) ( s->buff + s->head ) = 0;
        s->head = 0;
    }
    if ( s->cnt && s->head <= s->tail && s->head + jmpSize > s->tail )
    {
        assert( false );
        return NULL;
    }

    // The first sizeof(size_t) byte of allocated memory indicates next memory block location.
    void* ret = s->buff + s->head + sizeof( size_t );
    *( size_t*) ( s->buff + s->head ) = jmpSize; 

    s->head += jmpSize;
    ++s->cnt;

    return ret;
}

void queue_allocator_pop( struct queue_allocator* s )
{
    size_t jmpSize;

    if ( s->cnt == 0 )
    {
        assert( false );
        return;
    }

    size_t initTailPos = s->tail;
    jmpSize = *( size_t*) ( s->buff + s->tail );

    s->tail += jmpSize;
    --s->cnt;

    if ( *( size_t*) ( s->buff + s->tail ) == 0 )
    {
        // Always points valid value.
        s->tail = 0; 
    }
}

void* queue_allocator_peek( struct queue_allocator* s, size_t* size )
{
    assert( s->cnt && size );
    *size = *( size_t*) ( s->buff + s->tail ) - sizeof( size_t );
    return s->buff + s->tail + sizeof( size_t );
}
