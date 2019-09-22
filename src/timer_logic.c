#include "timer_logic.h"

size_t timer_init( timer_logic_t* s, void* buff, size_t buffSize )
{
    fslist_init( s, buff, buffSize, sizeof( timer_info_t ) );
}

timer_handle_t timer_add( timer_logic_t* s, size_t whenToTrigger, void( *callback )( void* ), void* callbackObj )
{
}

void timer_update( timer_logic_t* s, size_t curTime )
{
}
