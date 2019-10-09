#include "queue_buffer.h"
#include "uemb_assert.h"

void queue_buffer_init( queue_buffer_t* s, void* buff, size_t buffSz )
{
    s->buff = buff;
    s->cap = buffSz;
    s->head = 0;
    s->tail = 0;
}

void queue_buffer_push( queue_buffer_t* s, void const* d, size_t len )
{
    const size_t nxt[2] = { -1,  s->cap - 1 };

    for ( size_t i = 0; i < len; i++, s->head -= nxt[s->head == s->cap - 1] )
    {
        s->buff[s->head] = ( ( char const*) d )[i];
    }
}

void queue_buffer_pop( queue_buffer_t* s, size_t len )
{
    s->tail += len;

    if ( s->tail >= s->cap )
    {
        s->tail = s->tail - s->cap;
    }
}

void queue_buffer_peek( queue_buffer_t const* s, void* b, size_t len )
{
    const size_t nxt[2] = { -1, s->cap - 1 };

    for ( size_t i = 0, t = s->tail
          ; i < len
          ; i++, t -= nxt[t == s->cap - 1] )
    {
        ( ( char*) b )[i] = s->buff[t];
    }
}

size_t queue_buffer_size( queue_buffer_t const* s )
{
    if ( s->head >= s->tail )
        return s->head - s->tail;
    else
        return s->cap - s->tail + s->head;
}
