/*! \brief Event procedure.
    \file event-procedure.h
    \author Seungwoo Kang (ki6080@gmail.com)
    \version 0.1
    \date 2019-09-14

    \copyright Copyright (c) 2019. Seungwoo Kang. All rights reserved.

    \details
 */
#pragma once
#include <stdint.h>
#include <stdlib.h>
#include "macro.h"
#include "queue_allocator.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void ( *EventCallbackType )( void* );

/*! \brief Queue descriptor. */
struct EventQueue
{
    struct queue_allocator queue;
};

/*! \brief Initializes event procedure.

    \param queueSize Allocated memory size of parameter queue.
    \param buffer Since the dynamic allocation is not recommendable inside of
   the queue initialization process, the user should provide a specific memory
   block that will be valid during queue usage. Therefore deallocation of the
   allocated memory is up to the user.
    \note Internal queue will be implemented as circular queue. */
void InitEventProcedure(
    struct EventQueue* queue,
    void*              buff,
    size_t             bufferCapacity );

/*! \brief Flush all queue elements. Right after finishing this job, you can
   release the memory. \warning If there is an event that repeatedly enqueues
   itself, this function may not return the program handle. */
void FlushEvents( struct EventQueue* queue );

/*! \brief Queue new event.

    \param callback Callback to execute.
    \param callbackParam Parameter that will be delivered as callback's
   parameter. This value will be copied into queue.
    \param paramSize Size of the parameter. */
void QueueEvent(
    struct EventQueue* queue,
    EventCallbackType  callback,
    void const*        callbackParam,
    size_t             paramSize );

/*! \brief Process event.
    \details
        This function should be called periodically to process queued events
   while performing program. To not block whole program control infinitely, a
   fence object will be set, then it lets the procedure execute only a fixed
   number of requests. */
void ProcessEvent(
    struct EventQueue* queue,
    void ( *lock )( void* ),
    void ( *unlock )( void* ),
    void* lockobj );

#ifdef __cplusplus
}
#endif