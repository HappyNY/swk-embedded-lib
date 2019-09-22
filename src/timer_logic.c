#include "timer_logic.h"
#include "uemb_assert.h"

size_t timer_init( timer_logic_t* s, void* buff, size_t buffSize )
{
    size_t retval = fslist_init( &s->nodes, buff, buffSize, sizeof( timer_info_t ) );
    s->idGen = 0;
    return retval;
}

static inline
struct fslist_node* timer_find( timer_logic_t* s, size_t tick )
{
    struct fslist_node* head;
    
    if ( s->nodes.size == 0 )
        return NULL;

    head = s->nodes.get + s->nodes.head;

    while ( head )
    {
        // Finds place to insert new timer.
        if ( ( ( timer_info_t*) fslist_data( &s->nodes, head ) )->triggerTime >= tick )
            return head;
            
        head = fslist_next( &s->nodes, head );
    }

    return NULL;
}

timer_handle_t timer_add( timer_logic_t* s, size_t whenToTrigger, void( *callback )( void* ), void* callbackObj )
{
    struct fslist_node* n;
    timer_info_t* info;
    timer_handle_t ret;

    n = fslist_insert( &s->nodes, timer_find( s, whenToTrigger ) );
    uemb_assert( n );
    uemb_assert( callback );

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
    struct fslist_node* head, * tmp;
    timer_info_t* info;
    void ( *cb )( void* );
    void* obj;

    if ( s->nodes.size == 0 )
        return;

    for ( head = &s->nodes.get[s->nodes.head]
          ; head && ( *( info = ( timer_info_t*) fslist_data( &s->nodes, head ) ) ).triggerTime <= curTime
          ; )
    {
        tmp = head;
        head = fslist_next( &s->nodes, head );

        cb = info->callback;
        obj = info->callbackObj;

        fslist_erase( &s->nodes, tmp );

        cb( obj );
    }
}
