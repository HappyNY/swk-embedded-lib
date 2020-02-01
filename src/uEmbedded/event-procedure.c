#include "event-procedure.h"
#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#pragma pack( push, 4 )
struct queueArg
{
    EventCallbackType func;
};
#pragma pack( pop )

inline size_t bundleSize( size_t paramSize )
{
    return sizeof( struct queueArg ) + paramSize;
}

void InitEventProcedure(
    struct EventQueue* queue,
    void*              buff,
    size_t             bufferCapacity )
{
    queue_allocator_init( &queue->queue, buff, bufferCapacity );
}

void FlushEvents( struct EventQueue* queue )
{
    while ( queue->queue.cnt ) {
        ProcessEvent( queue, NULL, NULL, NULL );
    }
}

void QueueEvent(
    struct EventQueue* queue,
    EventCallbackType  callback,
    void const*        callbackParam,
    size_t             paramSize )
{
    struct queueArg* arg = (struct queueArg*)queue_allocator_push(
        &queue->queue,
        bundleSize( paramSize ) );
    arg->func  = callback;
    void* data = (void*)( arg + 1 );

    // Copy parameter data to buffer.
    if ( callbackParam && paramSize )
        memcpy( data, callbackParam, paramSize );
}

void ProcessEvent(
    struct EventQueue* queue,
    void ( *lock )( void* ),
    void ( *unlock )( void* ),
    void* lockobj )
{
    // Only a fixed number of events will be processed for each procedure call.
    size_t fence = queue->queue.cnt;
    size_t len;

    if ( lock && unlock )
        while ( fence-- ) {
            struct queueArg* arg
                = (struct queueArg*)queue_allocator_peek( &queue->queue, &len );

            // Call event
            arg->func( (void*)( arg + 1 ) );

            // Proceed parsing tail.
            lock( lockobj );
            queue_allocator_pop( &queue->queue );
            unlock( lockobj );
        }
    else
        while ( fence-- ) {
            struct queueArg* arg
                = (struct queueArg*)queue_allocator_peek( &queue->queue, &len );

            // Call event
            arg->func( (void*)( arg + 1 ) );

            // Proceed parsing tail.
            queue_allocator_pop( &queue->queue );
        }
}
