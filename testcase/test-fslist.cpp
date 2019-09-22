#include <Catch2/catch.hpp>

extern "C" {
#include <fslist.h>
}

TEST_CASE( "fslist functionalities.", "[fslist]" )
{
    fslist v;
    size_t cnt = fslist_init( &v, malloc( 1024 ), 1024, sizeof( double ) );

    REQUIRE( cnt == 1024 / ( sizeof( fslist_node ) + sizeof( double ) ) );
    REQUIRE( v.elemSize == sizeof( double ) );

    SECTION( "On adding first argument" )
    {
        auto n = fslist_insert( &v, NULL );
        REQUIRE( fslist_idx( &v, n ) == v.tail );
        REQUIRE( fslist_idx( &v, n ) == v.head );
        REQUIRE( v.size == 1 );
        REQUIRE( n->object );
        REQUIRE( n->next == FSLIST_NODEIDX_NONE );
        REQUIRE( n->prev == FSLIST_NODEIDX_NONE );
    }

    SECTION( "On list fullfilled" )
    {
        for ( size_t i = 0; i < cnt / 2; i++ )
        {
            auto n = fslist_insert( &v, NULL );
            REQUIRE( fslist_idx( &v, n ) == v.tail );
            REQUIRE( v.size == i * 2 + 1 );
            n = fslist_insert( &v, v.get + v.head );
            REQUIRE( fslist_idx( &v, n ) == v.head );
            REQUIRE( v.size == i * 2 + 2 );
        }

        REQUIRE( v.inactive == FSLIST_NODEIDX_NONE );
        REQUIRE_THROWS( fslist_insert( &v, NULL ) );

        SECTION( "On emptying list" )
        {
            while ( v.size )
            {
                auto idx = fslist_idx( &v, &v.get[v.head] );
                fslist_erase( &v, v.get + v.head );
                REQUIRE( v.inactive == idx );
            }
        }
    }
}