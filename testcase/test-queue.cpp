extern "C"
{
#include "queue_allocator.h"
#include "event-procedure.h"
}
#include <catch2/catch.hpp>
#include <string>
#include <vector>

static void gen_random( char* s, const int len ) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    for ( int i = 0; i < len; ++i ) {
        s[i] = alphanum[rand() % ( sizeof( alphanum ) - 1 )];
    }

    s[len] = 0;
}

TEST_CASE( "Queue functionality test", "[Queue]" )
{
    queue_allocator s;
    size_t const cap = 200000;
    queue_allocator_init( &s, malloc( cap ), cap );

    std::vector<std::string> init;
    char buff[128];

    for ( int lp = 0; lp < 128; ++lp )
    {
        init.clear();

        for ( int i = 0; i < 100; ++i )
        {
            gen_random( buff, ( rand() % ( sizeof( buff ) ) - 1 ) + 1 );
            init.push_back( buff );
        }

        for ( auto& str : init )
        {
            strcpy( ( char*) queue_allocator_push( &s, str.size() + 1 ), str.c_str() );
        }

        for ( auto& str : init )
        {
            size_t placeholder;
            auto a = ( char*) queue_allocator_peek( &s, &placeholder );
            auto b = str.data();
            CHECK( strcmp( a, b ) == 0 );
            queue_allocator_pop( &s );
        }
    }

    free( s.buff );
} 
 