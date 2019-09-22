#include "timer_logic.h"
#include "uemb_assert.h"

size_t timer_init( timer_logic_t* s, void* buff, size_t buffSize )
{
    size_t retval = fslist_init( s, buff, buffSize, sizeof( timer_info_t ) );
    s->idGen = 0;
    return retval;
}

static inline
struct fslist_node* timer_find( timer_logic_t* s, size_t tick )
{

}

timer_handle_t timer_add( timer_logic_t* s, size_t whenToTrigger, void( *callback )( void* ), void* callbackObj )
{
    struct fslist_node* n;
    timer_info_t* info;
    timer_handle_t ret;

    n = fslist_insert( &s->nodes, timer_find( s, whenToTrigger ) );
    uemb_assert( n );

    info = ( timer_info_t*) fslist_data( &s->nodes, n );
    
    info->callback = callback;
    info->callbackObj = callbackObj;
    info->timerId = s->idGen++;
    info->triggerTime = whenToTrigger;
 
    ret.n = n;
    ret.timerId = info->timerId;
    return ret;
}

void timer_update( timer_logic_t* s, size_t curTime )
{

}
