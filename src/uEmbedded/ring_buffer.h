#pragma once
#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
     
/*! @breif		Circular queue buffer.
    @details
        Since circular queue does not ensure a single memory chunk to be
   continuous, the queue buffer provides peek operation as copying memory chunk
   internally. Thus it can be extremely slow when peeking large memory chunks
   repeatedly, It is recommended to use this functionality only for holding data
   temporarily. This class differs from queue_allocator in terms of element
   handling. queue_allocator makes an element distinguishable by wrapping it
   with its size. On the other hand, this class works like a byte stream which
   does not contain any information about the original element.

    @warning
        There is no safeguard for overflow to maximize embedded performance.
*/
struct ring_buffer {
    //!
    char *buff;

    //! Buffer capacity.
    size_t cap;

    //! Head indicator
    size_t head;

    //! Tail indiciator
    size_t tail;
};

//! Alias of class
typedef struct ring_buffer ring_buffer_t;

/*! \breif		Initiate buffer */
void ring_buffer_init(ring_buffer_t *s, void *buff, size_t buffSz);

/*! \breif		Push data into queue buffer */
void ring_buffer_write(ring_buffer_t *s, void const *d, size_t len);

/*! \breif		Pop data from queue buffer. */
void ring_buffer_consume(ring_buffer_t *s, size_t len);

/*! \breif		Peek data from queue buffer. */
void ring_buffer_peek(ring_buffer_t const *s, void *b, size_t len);

/*! \breif      Get current data cnt */
size_t ring_buffer_size(ring_buffer_t const *s);

/*! \breif      Do peeking and consuming at once.
    \return     Number of byte actually read. */
static inline size_t ring_buffer_read(ring_buffer_t *s, void *b, size_t len)
{
    size_t sz = ring_buffer_size(s);
    if (sz < len)
        len = sz;
    ring_buffer_peek(s, b, len);
    ring_buffer_consume(s, len);
    return len;
}
 

#ifdef __cplusplus
}
#endif