#include "event-procedure.h"
#include <stdlib.h> 
#include <assert.h>
#include <stdbool.h>
#include <string.h>


//! \todo. Verify functionalities.

#pragma pack(push, 4)
struct queueArg
{
    EventCallbackType func;
};
#pragma pack(pop)

inline size_t bundleSize( size_t paramSize )
{
    return sizeof( struct queueArg ) + paramSize;
}

void InitEventProcedure( struct EventQueue* queue, void* buff, size_t bufferCapacity )
{
    queue_allocator_init( &queue->queue, buff, bufferCapacity );
}

void FlushEvents( struct EventQueue* queue )
{
    while ( queue->queue.cnt )
    {
        ProcessEvent( queue );
    }
}

void QueueEvent( struct EventQueue* queue, EventCallbackType callback, void const* callbackParam, size_t paramSize )
{ 
    struct queueArg* arg = ( struct queueArg* )queue_allocator_push( &queue->queue, bundleSize( paramSize ) );
    arg->func = callback;
    void* data = ( void*) ( arg + 1 );

    // Copy parameter data to buffer.
    memcpy( data, callbackParam, paramSize );
}

void ProcessEvent( struct EventQueue* queue )
{
    // Only a fixed number of events will be processed for each procedure call.
    size_t fence = queue->queue.head;
    size_t len;

    while ( queue->queue.tail != fence )
    {
        struct queueArg* arg = ( struct queueArg* )queue_allocator_peek( &queue->queue, &len );

        // Call event
        arg->func( ( void*) ( arg + 1 ) );

        // Proceed parsing tail.
        queue_allocator_pop( &queue->queue );
    }
}
