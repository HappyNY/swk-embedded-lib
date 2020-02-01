#include "ring_buffer.h"
#include <stdlib.h>
#include "uassert.h"

void ring_buffer_init( ring_buffer_t* s, void* buff, size_t buffSz )
{
    s->buff = buff;
    s->cap  = buffSz;
    s->head = 0;
    s->tail = 0;
}

void ring_buffer_write( ring_buffer_t* s, void const* d, size_t len )
{
    const size_t nxt[2] = { -1, s->cap - 1 };
    size_t       i;

    for ( i = 0; i < len; i++, s->head -= nxt[s->head == s->cap - 1] ) {
        s->buff[s->head] = ( (char const*)d )[i];
    }
}

void ring_buffer_consume( ring_buffer_t* s, size_t len )
{
    s->tail += len;

    if ( s->tail >= s->cap ) {
        s->tail = s->tail - s->cap;
    }
}

void ring_buffer_peek( ring_buffer_t const* s, void* b, size_t len )
{
    const size_t nxt[2] = { -1, s->cap - 1 };
    size_t       i, t;

    for ( i = 0, t = s->tail; i < len; i++, t -= nxt[t == s->cap - 1] ) {
        ( (char*)b )[i] = s->buff[t];
    }
}

size_t ring_buffer_size( ring_buffer_t const* s )
{
    if ( s->head >= s->tail )
        return s->head - s->tail;
    else
        return s->cap - s->tail + s->head;
}
