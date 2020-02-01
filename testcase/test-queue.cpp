extern "C"
{
#include "uEmbedded/queue_allocator.h"
#include "uEmbedded/event-procedure.h"
#include "uEmbedded/ring_buffer.h"
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
    size_t const cap = 150000;
    queue_allocator_init( &s, malloc( cap ), cap );

    std::vector<std::string> init;
    char buff[128];

    size_t onCount = 0;
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
            strcpy_s( ( char*) queue_allocator_push( &s, str.size() + 1 ), str.size() + 1, str.c_str() );
        }

        for ( auto& str : init )
        {
            ++onCount;
            
            size_t placeholder;
            auto a = ( char*) queue_allocator_peek( &s, &placeholder );
            auto b = str.data();
            INFO("A: " << a << "\n");
            INFO("B: " << b << "\n");
            INFO("On count: " << onCount << "\n");
            INFO( s.cnt << " " << lp );
            REQUIRE( strcmp( a, b ) == 0 );
            queue_allocator_pop( &s );
        }
    }

    free( s.buff );
} 

using namespace std;

TEST_CASE( "Buffer test", "[ring_buffer]" )
{
    static char buff[0x10000];
    ring_buffer_t v;

    ring_buffer_init( &v, buff, sizeof( buff ) );

    // @todo.
}