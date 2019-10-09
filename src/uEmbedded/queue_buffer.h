#pragma once
#include <stdlib.h>
#include <stdint.h>

/*! @breif		Circular queue buffer. 
	@details	
		Since circular queue does not ensure a single memory chunk to be continuous, the queue buffer provides peek operation as copying memory chunk internally.
		Thus it can be extremely slow when peeking large memory chunks repeatedly, It is recommended to use this functionality only for holding data temporarily.
        This class differs from queue_allocator in terms of element handling. queue_allocator makes an element distinguishable by wrapping it with its size. On the other hand, this class works like a byte stream which does not contain any information about the original element.

    @warning    
        There is no safeguard for overflow to maximize embedded performance.
*/
struct queue_buffer
{
	//! 
    char* buff;

	//! Buffer capacity.
    size_t cap;

	//! Head indicator
    size_t head;

	//! Tail indiciator
    size_t tail;
};

//! Alias of class
typedef struct queue_buffer queue_buffer_t;


/*! \breif		Initiate buffer */
void queue_buffer_init( queue_buffer_t* s, void* buff, size_t buffSz );

/*! \breif		Push data into queue buffer */
void queue_buffer_push( queue_buffer_t* s, void const* d, size_t len );

/*! \breif		Pop data from queue buffer. */
void queue_buffer_pop( queue_buffer_t* s, size_t len );

/*! \breif		Peek data from queue buffer. */
void queue_buffer_peek( queue_buffer_t const* s, void* b, size_t len );

/*! \breif      Get current data cnt */
size_t queue_buffer_size( queue_buffer_t const* s );

/*! \breif      Do popping and peeking at once. 
    \return     Number of byte actually read. */
static inline 
size_t queue_buffer_draw( queue_buffer_t* s, void* b, size_t len )
{
    size_t sz = queue_buffer_size( s ); 
    if ( sz < len )
        len = sz;
    queue_buffer_peek( s, b, len );
    queue_buffer_pop( s, len );
    return len;
}
