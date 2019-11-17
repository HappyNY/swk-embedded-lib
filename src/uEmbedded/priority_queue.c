#include "priority_queue.h"
#include "uassert.h"
#include <string.h>

void pqueue_init(struct priority_queue* s, size_t elemSize, void* buff, size_t buffSize, int( *pred )( void const*, void const* ))
{
    uassert(s && elemSize && buff && pred);
    s->pred = pred;
    s->cnt = 0;
    s->elemSize = elemSize;
    s->capacity = buffSize / elemSize;
    uassert(s->capacity);
    s->buff = buff;
}

#define get_at(s, idx) ((s)->buff + (idx) * (s)->elemSize)

static inline void memswap(void* a, void* b, size_t sz)
{
    char tmp; 

    while ( sz-- )
    {
        tmp = *( char*) a;
        *( char*) a = *( char*) b;
        *( char*) b = tmp;
        a = ( char*) a + 1;
        b = ( char*) b + 1;
    }
}


void pqueue_push(struct priority_queue* s, void const* elem)
{
    uassert(s && elem);
    uassert(s->cnt < s->capacity);

    size_t idx = s->cnt;
    memcpy(get_at(s, idx), elem, s->elemSize);

    char* a, * b;

    for ( ;;)
    {
        size_t up = ( idx - 1 ) >> 1;
        a = get_at(s, up);
        b = get_at(s, idx);

        // Escape condition. If parent node is smaller than or equal with current ...
        if ( s->pred(a, b) <= 0 )
            break;

        // Swap memory one by one
        memswap(a, b, s->elemSize);

        idx = up;
    }

    s->cnt++;
}

void pqueue_pop(struct priority_queue* s)
{
    uassert(s);
    uassert(s->cnt);

    --s->cnt;
    memcpy(s->buff, get_at(s, s->cnt), s->elemSize);
    size_t idx = 0;
    size_t nxt, b;
    char* p[3];

    for ( ;;)
    {
        nxt = ( idx << 1 ) + 1;
        b = ( idx << 1 ) + 2;

        p[0] = get_at(s, idx);
        p[1] = get_at(s, nxt);
        p[2] = get_at(s, b);
        
        if ( b < s->cnt )
            nxt += ( s->pred(p[1], p[2]) > 0 ); // Sellect lesser one.
        else if ( nxt >= s->cnt )
            break;

        p[1] = get_at(s, nxt);
        if ( s->pred(p[0], p[1]) <= 0 )
            break; //done.

        memswap(p[0], p[1], s->elemSize);
        idx = nxt;
    }
} 

